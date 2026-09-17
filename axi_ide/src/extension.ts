import * as vscode from 'vscode';
import { ScintillaPanel } from './scintillaPanel';

export function activate(context: vscode.ExtensionContext) {
    console.log('Congratulations, your extension "axi-ide" is now active!');

    let disposable = vscode.commands.registerCommand('axi-ide.openScintilla', () => {
        ScintillaPanel.createOrShow(context.extensionUri);
    });

    context.subscriptions.push(disposable);
}

export function deactivate() {}
