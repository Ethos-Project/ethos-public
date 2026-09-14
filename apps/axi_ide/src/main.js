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
const runBtn = document.getElementById('runBtn');

let saveTimeout = null;

editor.onDidChangeModelContent(() => {
    if (activeFilePath && openFiles[activeFilePath]) {
        openFiles[activeFilePath].content = editor.getValue();
        // Auto-save debounce
        clearTimeout(saveTimeout);
        statusEl.innerText = 'Saving...';
        saveTimeout = setTimeout(async () => {
            try {
                await invoke('write_file', { path: activeFilePath, contents: openFiles[activeFilePath].content });
                statusEl.innerText = 'Saved.';
                setTimeout(() => { if (statusEl.innerText === 'Saved.') statusEl.innerText = 'Ready'; }, 1500);
            } catch (e) {
                term.writeln(`\x1b[31mAuto-save failed: ${e}\x1b[0m`);
            }
        }, 800);
    }
});

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
        
        const icon = document.createElement('i');
        icon.className = `file-icon codicon ${node.is_dir ? 'codicon-folder' : 'codicon-file'}`;
        
        const text = document.createElement('span');
        text.innerText = node.name;
        
        el.appendChild(icon);
        el.appendChild(text);
        
        if (!node.is_dir) {
            el.addEventListener('click', () => openFile(node.path, node.name));
        }
        container.appendChild(el);
    });
}

// 5. Tabs and Editor Logic
async function openFile(path, name) {
    if (!openFiles[path]) {
        try {
            const content = await invoke('read_file', { path });
            const model = monaco.editor.createModel(content, path.endsWith('.axi') ? 'axi' : (path.endsWith('.py') ? 'python' : (path.endsWith('.c') || path.endsWith('.cpp') ? 'cpp' : (path.endsWith('.cs') ? 'csharp' : 'plaintext'))));
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

function getCompilerText(path) {
    if (!path) return '<i class="codicon codicon-play"></i> Compile to Safe C';
    if (path.endsWith('.axi')) return '<i class="codicon codicon-play"></i> Compile Axi';
    if (path.endsWith('.py')) return '<i class="codicon codicon-play"></i> Compile Python to C';
    if (path.endsWith('.cs')) return '<i class="codicon codicon-play"></i> Compile C# to C';
    if (path.endsWith('.js') || path.endsWith('.ts')) return '<i class="codicon codicon-play"></i> Compile JS to C';
    return '<i class="codicon codicon-play"></i> Compile to Safe C';
}

function setActiveFile(path) {
    activeFilePath = path;
    if (path && openFiles[path]) {
        editor.setModel(openFiles[path].model);
        
        const isCompilable = path.endsWith('.axi') || path.endsWith('.py') || path.endsWith('.c') || path.endsWith('.cpp') || path.endsWith('.cs') || path.endsWith('.js') || path.endsWith('.ts');
        compileBtn.disabled = !isCompilable;
        compileBtn.innerHTML = getCompilerText(path);
    } else {
        editor.setModel(null);
        compileBtn.disabled = true;
        compileBtn.innerHTML = '<i class="codicon codicon-play"></i> Compile to Safe C';
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

let lastCompiledExe = null;

compileBtn.addEventListener('click', async () => {
    if (!activeFilePath) return;
    statusEl.innerText = 'Compiling...';
    term.writeln(`\x1b[33mCompiling ${activeFilePath}...\x1b[0m`);
    
    try {
        const result = await invoke('compile_axi_code', { filePath: activeFilePath });
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
