#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// =====================================================================
// EROS MCP REGISTRY: Full Agentic Tool Suite
// =====================================================================

typedef struct {
    HANDLE hChildStd_IN_Rd;
    HANDLE hChildStd_IN_Wr;
    HANDLE hChildStd_OUT_Rd;
    HANDLE hChildStd_OUT_Wr;
    PROCESS_INFORMATION piProcInfo;
    char tool_name[64];
    int is_active;
} MCPProcess;

typedef struct {
    MCPProcess tools[10];
    int tool_count;
} MCPRegistry;

// Generic function to spawn an MCP executable via Anonymous Pipes
static inline MCPProcess spawn_mcp_subprocess(const char* name, const char* exe_path) {
    MCPProcess mcp = {0};
    strncpy(mcp.tool_name, name, 63);
    SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    CreatePipe(&mcp.hChildStd_OUT_Rd, &mcp.hChildStd_OUT_Wr, &saAttr, 0);
    SetHandleInformation(mcp.hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);
    CreatePipe(&mcp.hChildStd_IN_Rd, &mcp.hChildStd_IN_Wr, &saAttr, 0);
    SetHandleInformation(mcp.hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA siStartInfo;
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFOA));
    siStartInfo.cb = sizeof(STARTUPINFOA);
    siStartInfo.hStdError = mcp.hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = mcp.hChildStd_OUT_Wr;
    siStartInfo.hStdInput = mcp.hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    BOOL bSuccess = CreateProcessA(NULL, (LPSTR)exe_path, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &mcp.piProcInfo);
    
    mcp.is_active = bSuccess ? 1 : 0;
    if (bSuccess) printf("[MCP REGISTRY] Successfully mounted '%s'\n", name);
    else printf("[MCP REGISTRY] Failed to mount '%s'\n", name);
    
    return mcp;
}

// Mounts all discovered MCP tools from the AllosPHP IPC folder
static inline MCPRegistry mcp_mount_all_tools() {
    printf("[EROS BUS] Initializing Global MCP Tool Registry...\n");
    MCPRegistry registry = {0};
    
    registry.tools[0] = spawn_mcp_subprocess("sequential_thinking", "C:\\Ethos\\allos\\allos-php\\ipc\\sequential_thinking_mcp.allos.exe");
    registry.tools[1] = spawn_mcp_subprocess("webtools", "C:\\Ethos\\allos\\allos-php\\ipc\\webtools_mcp.allos.exe");
    registry.tools[2] = spawn_mcp_subprocess("cloudrun", "C:\\Ethos\\allos\\allos-php\\ipc\\cloudrun_mcp.allos.exe");
    registry.tools[3] = spawn_mcp_subprocess("genkit", "C:\\Ethos\\allos\\allos-php\\ipc\\genkit_mcp.allos.exe");
    registry.tool_count = 4;
    
    printf("[EROS BUS] All MCP Tools Bound and Standing By.\n\n");
    return registry;
}

// Dynamic routing: Pipes Eros's action to the correct MCP server
static inline void mcp_route_action(MCPRegistry* registry, const char* target_tool, const char* payload, char* response_buffer) {
    for (int i = 0; i < registry->tool_count; i++) {
        if (registry->tools[i].is_active && strcmp(registry->tools[i].tool_name, target_tool) == 0) {
            DWORD dwWritten, dwRead;
            // Write action to the tool
            WriteFile(registry->tools[i].hChildStd_IN_Wr, payload, strlen(payload), &dwWritten, NULL);
            WriteFile(registry->tools[i].hChildStd_IN_Wr, "\n", 1, &dwWritten, NULL);
            
            // Read result from the tool
            BOOL bSuccess = ReadFile(registry->tools[i].hChildStd_OUT_Rd, response_buffer, 4096, &dwRead, NULL);
            if (bSuccess && dwRead > 0) response_buffer[dwRead] = '\0';
            return;
        }
    }
    sprintf(response_buffer, "ERROR: Tool '%s' not found or inactive.", target_tool);
}
