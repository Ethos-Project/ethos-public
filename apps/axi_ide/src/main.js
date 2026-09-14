import * as monaco from 'monaco-editor';
import { invoke } from '@tauri-apps/api/tauri';

// Initialize Monaco Editor
const editor = monaco.editor.create(document.getElementById('editor-container'), {
    value: 'HOW TO start():\n    WRITE "Hello from Axi IDE!" TO SCREEN\n\nstart -> start',
    language: 'plaintext',
    theme: 'vs-dark',
    automaticLayout: true,
    fontSize: 14,
    fontFamily: 'Consolas, "Courier New", monospace'
});

// Hook up the compile button to the Rust/C++ backend
document.getElementById('compileBtn').addEventListener('click', async () => {
    const statusEl = document.getElementById('status');
    statusEl.innerText = 'Compiling...';
    
    const sourceCode = editor.getValue();
    
    try {
        // Send code to Tauri Rust backend, which forwards to axi_compiler.dll
        const result = await invoke('compile_axi_code', { sourceCode });
        statusEl.innerText = `Success! Output: ${result}`;
        statusEl.style.color = '#4caf50';
    } catch (error) {
        statusEl.innerText = `Error: ${error}`;
        statusEl.style.color = '#f44336';
    }
});
