import * as vscode from 'vscode';

export class ScintillaPanel {
    public static currentPanel: ScintillaPanel | undefined;
    public static readonly viewType = 'scintillaEditor';
    private readonly _panel: vscode.WebviewPanel;
    private _disposables: vscode.Disposable[] = [];

    public static createOrShow(extensionUri: vscode.Uri) {
        const column = vscode.window.activeTextEditor ? vscode.window.activeTextEditor.viewColumn : undefined;
        if (ScintillaPanel.currentPanel) {
            ScintillaPanel.currentPanel._panel.reveal(column);
            return;
        }
        const panel = vscode.window.createWebviewPanel(ScintillaPanel.viewType, 'Axi Scintilla', column || vscode.ViewColumn.One, {
            enableScripts: true
        });
        ScintillaPanel.currentPanel = new ScintillaPanel(panel, extensionUri);
    }

    private constructor(panel: vscode.WebviewPanel, extensionUri: vscode.Uri) {
        this._panel = panel;
        this._panel.onDidDispose(() => this.dispose(), null, this._disposables);
        this._panel.webview.html = this._getHtmlForWebview();
    }

    public dispose() {
        ScintillaPanel.currentPanel = undefined;
        this._panel.dispose();
        while (this._disposables.length) {
            const x = this._disposables.pop();
            if (x) { x.dispose(); }
        }
    }

    private _getHtmlForWebview() {
        return <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Scintilla Webview</title>
            <style>
                body { padding: 0; margin: 0; height: 100vh; display: flex; flex-direction: column; }
                #editor { flex: 1; border: none; background: #1e1e1e; color: #d4d4d4; padding: 10px; font-family: monospace; }
            </style>
        </head>
        <body>
            <!-- Scintilla-based editor placeholder -->
            <textarea id="editor" spellcheck="false">// Axi Scintilla WebAssembly Port Placeholder
// The native Scintilla engine will be hosted here via WASM.</textarea>
        </body>
        </html>;
    }
}
