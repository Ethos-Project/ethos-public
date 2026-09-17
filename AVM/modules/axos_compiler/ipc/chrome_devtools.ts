// chrome_devtools.ts
// Native TypeScript implementation designed for Axi JIT/AOT Transpilation
// This file uses NO Node.js dependencies. It compiles directly into Winsock2 C-bindings.

// Assume native Axi transpiler injects these low-level bindings:
declare namespace axi {
    function http_get(url: string): string;
    function ws_connect(url: string): any;
    function ws_send(socket: any, payload: string): string;
    function ws_close(socket: any): void;
}

export function get_chrome_devtools_list(): string {
    return `{
        "tools": [
            {
                "name": "navigate",
                "description": "Navigate the browser to a specific URL using pure CDP",
                "inputSchema": { "type": "object", "properties": { "url": { "type": "string" } } }
            },
            {
                "name": "evaluate_js",
                "description": "Evaluate arbitrary JavaScript on the active page via CDP",
                "inputSchema": { "type": "object", "properties": { "script": { "type": "string" } } }
            }
        ]
    }`;
}

// Helper to negotiate the active Chrome CDP WebSocket URL
function get_active_cdp_ws_url(): string {
    try {
        const response = axi.http_get("http://127.0.0.1:9222/json/list");
        // Parse the raw JSON payload natively
        // Looks like: [ { "webSocketDebuggerUrl": "ws://127.0.0.1:9222/devtools/page/..." } ]
        const ws_url_match = response.match(/"webSocketDebuggerUrl":\s*"([^"]+)"/);
        if (ws_url_match && ws_url_match[1]) {
            return ws_url_match[1];
        }
    } catch (e) {
        return "";
    }
    return "";
}

export function execute_devtools_action(tool_name: string, params: any): string {
    const ws_url = get_active_cdp_ws_url();
    if (!ws_url) {
        return `{ "error": "Could not connect to Chrome on localhost:9222. Is it running with --remote-debugging-port=9222?" }`;
    }

    const socket = axi.ws_connect(ws_url);
    if (!socket) {
        return `{ "error": "Failed to open native WebSocket binding to Chrome CDP." }`;
    }

    let result = "";

    if (tool_name === "navigate") {
        const url = params.arguments.url;
        const cdp_payload = `{"id": 1, "method": "Page.navigate", "params": {"url": "${url}"}}`;
        const cdp_response = axi.ws_send(socket, cdp_payload);
        
        result = `{ "content": [{ "type": "text", "text": "Successfully dispatched Page.navigate to ${url}. CDP Response: ${cdp_response}" }] }`;
    } 
    else if (tool_name === "evaluate_js") {
        const script = params.arguments.script;
        const cdp_payload = `{"id": 2, "method": "Runtime.evaluate", "params": {"expression": "${script}", "returnByValue": true}}`;
        const cdp_response = axi.ws_send(socket, cdp_payload);
        
        result = `{ "content": [{ "type": "text", "text": "CDP Runtime.evaluate response: ${cdp_response}" }] }`;
    } else {
        result = `{ "error": "Unknown tool executed." }`;
    }

    axi.ws_close(socket);
    return result;
}
