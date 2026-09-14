import * as monaco from 'monaco-editor';
import { invoke } from '@tauri-apps/api/tauri';
import { Terminal } from 'xterm';
import { FitAddon } from 'xterm-addon-fit';
import { open } from '@tauri-apps/api/dialog';

// State
let currentWorkspace = null;
let activeFilePath = null;
let openFiles = {}; // { path: { content, model } }

// 1. Setup Xterm.js Terminal
const term = new Terminal({
    theme: { background: '#1e1e1e' },
    fontFamily: 'Consolas, "Courier New", monospace',
    fontSize: 13,
    cursorBlink: true
});
const fitAddon = new FitAddon();
term.loadAddon(fitAddon);
term.open(document.getElementById('terminal-container'));
fitAddon.fit();
window.addEventListener('resize', () => fitAddon.fit());
term.writeln('\x1b[32mWelcome to Axi IDE (FOSS Edition) Terminal\x1b[0m');

let termCommand = '';
let termCwd = '';

function printPrompt() {
    const cwdDisplay = termCwd || '~';
    term.write(`\r\n\x1b[32m${cwdDisplay}\x1b[0m> `);
}

term.onData(async e => {
    switch (e) {
        case '\r': // Enter
            term.writeln('');
            if (termCommand.trim() !== '') {
                try {
                    const output = await invoke('run_shell', { cmd: termCommand, cwd: termCwd });
                    const lines = output.replace(/\r\n/g, '\n').split('\n');
                    lines.forEach(line => term.writeln(line));
                } catch (err) {
                    term.writeln(`\x1b[31m${err}\x1b[0m`);
                }
            }
            termCommand = '';
            printPrompt();
            break;
        case '\u007F': // Backspace
            if (termCommand.length > 0) {
                termCommand = termCommand.substr(0, termCommand.length - 1);
                term.write('\b \b');
            }
            break;
        default:
            if (e >= String.fromCharCode(0x20) && e <= String.fromCharCode(0x7E) || e >= '\u00a0') {
                termCommand += e;
                term.write(e);
            }
    }
});
printPrompt();

// 2. Setup Monaco Editor & Axi Syntax
monaco.languages.register({ id: 'axi' });
monaco.languages.setMonarchTokensProvider('axi', {
    tokenizer: {
        root: [
            [/HOW TO/, 'keyword'],
            [/WRITE|TO|SCREEN/, 'keyword'],
            [/->|=>/, 'operator'],
            [/".*?"/, 'string'],
        ]
    }
});

const editor = monaco.editor.create(document.getElementById('editor-container'), {
    theme: 'vs-dark',
    automaticLayout: true,
    fontSize: 14,
    fontFamily: 'Consolas, "Courier New", monospace'
});

// 3. UI Elements
const treeEl = document.getElementById('file-tree');
const tabsEl = document.getElementById('tabs');
const statusEl = document.getElementById('status');
const compileBtn = document.getElementById('compileBtn');
const saveBtn = document.getElementById('saveBtn');
const runBtn = document.getElementById('runBtn');

// 4. File Tree Logic
async function loadDirectory(path) {
    currentWorkspace = path;
    try {
        const nodes = await invoke('read_dir', { path });
        renderFileTree(nodes, path, treeEl);
    } catch (e) {
        term.writeln(`\x1b[31mFailed to read directory: ${e}\x1b[0m`);
    }
}

function renderFileTree(nodes, parentPath, container, depth = 0) {
    container.innerHTML = '';
    nodes.forEach(node => {
        const el = document.createElement('div');
        el.className = 'file-node';
        el.style.paddingLeft = `${10 + depth * 15}px`;
        
        const icon = document.createElement('span');
        icon.className = 'file-icon';
        icon.innerText = node.is_dir ? '?' : '?';
        
        const text = document.createElement('span');
        text.innerText = node.name;
        
        el.appendChild(icon);
        el.appendChild(text);
        
        if (!node.is_dir) {
            el.addEventListener('click', () => openFile(node.path, node.name));
        }
        // Directories could be expandable, but keep it flat/simple for v1 or just click to dive.
        container.appendChild(el);
    });
}

// 5. Tabs and Editor Logic
async function openFile(path, name) {
    if (!openFiles[path]) {
        try {
            const content = await invoke('read_file', { path });
            const model = monaco.editor.createModel(content, path.endsWith('.axi') ? 'axi' : 'plaintext');
            openFiles[path] = { name, content, model };
            renderTabs();
        } catch (e) {
            term.writeln(`\x1b[31mFailed to read file: ${e}\x1b[0m`);
            return;
        }
    }
    setActiveFile(path);
}

function renderTabs() {
    tabsEl.innerHTML = '';
    Object.keys(openFiles).forEach(path => {
        const file = openFiles[path];
        const tab = document.createElement('div');
        tab.className = `tab ${path === activeFilePath ? 'active' : ''}`;
        
        const title = document.createElement('span');
        title.innerText = file.name;
        
        const closeBtn = document.createElement('span');
        closeBtn.className = 'tab-close';
        closeBtn.innerText = '×';
        closeBtn.onclick = (e) => {
            e.stopPropagation();
            closeFile(path);
        };
        
        tab.appendChild(title);
        tab.appendChild(closeBtn);
        tab.onclick = () => setActiveFile(path);
        tabsEl.appendChild(tab);
    });
}

function setActiveFile(path) {
    activeFilePath = path;
    if (path && openFiles[path]) {
        editor.setModel(openFiles[path].model);
        saveBtn.disabled = false;
        compileBtn.disabled = !path.endsWith('.axi');
    } else {
        editor.setModel(null);
        saveBtn.disabled = true;
        compileBtn.disabled = true;
    }
    renderTabs();
}

function closeFile(path) {
    if (openFiles[path].model) openFiles[path].model.dispose();
    delete openFiles[path];
    if (activeFilePath === path) {
        const remaining = Object.keys(openFiles);
        setActiveFile(remaining.length > 0 ? remaining[0] : null);
    } else {
        renderTabs();
    }
}

// 6. Commands
document.getElementById('openFolderBtn').addEventListener('click', async () => {
    const selected = await open({ directory: true });
    if (selected) {
        termCwd = selected;
        term.writeln(`\x1b[34mOpened workspace: ${selected}\x1b[0m`);
        loadDirectory(selected);
        printPrompt();
    }
});

saveBtn.addEventListener('click', async () => {
    if (!activeFilePath) return;
    try {
        const content = editor.getValue();
        await invoke('write_file', { path: activeFilePath, contents: content });
        openFiles[activeFilePath].content = content;
        statusEl.innerText = 'Saved.';
        setTimeout(() => statusEl.innerText = 'Ready', 2000);
    } catch (e) {
        term.writeln(`\x1b[31mFailed to save: ${e}\x1b[0m`);
    }
});

let lastCompiledExe = null;

compileBtn.addEventListener('click', async () => {
    if (!activeFilePath) return;
    statusEl.innerText = 'Compiling...';
    term.writeln(`\x1b[33mCompiling ${activeFilePath}...\x1b[0m`);
    
    try {
        const sourceCode = editor.getValue();
        const result = await invoke('compile_axi_code', { sourceCode });
        term.writeln(`\x1b[32mSuccess! Compiled to: ${result}\x1b[0m`);
        statusEl.innerText = 'Build Successful';
        lastCompiledExe = result;
        runBtn.disabled = false;
    } catch (error) {
        term.writeln(`\x1b[31m${error}\x1b[0m`);
        statusEl.innerText = 'Build Failed';
    }
});

runBtn.addEventListener('click', async () => {
    if (!lastCompiledExe) return;
    term.writeln(`\x1b[36m> Executing ${lastCompiledExe}\x1b[0m`);
    try {
        const output = await invoke('execute_program', { path: lastCompiledExe });
        term.writeln(output);
        term.writeln(`\x1b[32mProgram exited successfully.\x1b[0m`);
    } catch (e) {
        term.writeln(`\x1b[31m${e}\x1b[0m`);
    }
});
