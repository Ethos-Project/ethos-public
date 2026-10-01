#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "eros_bus.h"

// =====================================================================
// MCP UPLINK: Sequential Thinking Engine
// Architecture: Subprocess IPC via Windows Anonymous Pipes
// =====================================================================

typedef struct {
    HANDLE hChildStd_IN_Rd;
    HANDLE hChildStd_IN_Wr;
    HANDLE hChildStd_OUT_Rd;
    HANDLE hChildStd_OUT_Wr;
    PROCESS_INFORMATION piProcInfo;
} MCPProcess;

// Spawns the sequential_thinking_mcp.axi.exe and maps its I/O directly to the ErosBus
static inline MCPProcess mcp_mount_sequential_thinking() {
    printf("[MCP UPLINK] Mounting Sequential Thinking Engine...\n");

    MCPProcess mcp = {0};
    SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    // Create pipes for standard output and input
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

    // Launch the pre-compiled MCP server from AllosPHP/ipc
    const char* cmd = "C:\\Ethos\\logos\\allos\\AllosPHP\\ipc\\sequential_thinking_mcp.axi.exe";
    BOOL bSuccess = CreateProcessA(NULL, (LPSTR)cmd, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &mcp.piProcInfo);
    
    if (bSuccess) {
        printf("[MCP UPLINK] Sequential Thinking MCP Successfully Bound to Eros Cortex.\n");
    } else {
        printf("[MCP UPLINK] Critical Failure: Could not bind MCP Server.\n");
    }

    return mcp;
}

// Pipes Eros's raw entropy into the MCP and waits for logical validation
static inline void mcp_execute_thought_step(MCPProcess* mcp, const char* thought_payload, char* mcp_response_buffer) {
    DWORD dwWritten, dwRead;
    
    // Write Eros's thought to the MCP's Standard Input
    WriteFile(mcp->hChildStd_IN_Wr, thought_payload, strlen(thought_payload), &dwWritten, NULL);
    WriteFile(mcp->hChildStd_IN_Wr, "\n", 1, &dwWritten, NULL);
    
    // Read the evaluated logic back from the MCP's Standard Output
    bSuccess = ReadFile(mcp->hChildStd_OUT_Rd, mcp_response_buffer, 4096, &dwRead, NULL);
    if (bSuccess && dwRead > 0) {
        mcp_response_buffer[dwRead] = '\0';
    }
}
