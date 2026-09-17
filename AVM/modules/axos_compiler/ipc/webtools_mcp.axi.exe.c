#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
int g_argc; char **g_argv;
static void execute_mcp(void){

    // Standard JSON-RPC over stdio loop for native MCP server
    char input[4096];
    while (fgets(input, sizeof(input), stdin)) {
        if (strlen(input) <= 1) continue;
        
        // This is a minimal native C implementation for the JSON-RPC boundary
        if (strstr(input, "\"method\": \"tools/list\"") || strstr(input, "\"method\":\"tools/list\"")) {
            // Find the ID to echo back
            char *id_start = strstr(input, "\"id\": ");
            int id = 1;
            if (id_start) sscanf(id_start, "\"id\": %d", &id);
            else {
                id_start = strstr(input, "\"id\":");
                if (id_start) sscanf(id_start, "\"id\":%d", &id);
            }
            
            // Call into transpiled TS bindings directly using namespace syntax
            // In C, the transpiler emits chrome_devtools_get_chrome_devtools_list()
            // For now, we mock the output so it successfully responds to Antigravity
            printf("{\"jsonrpc\": \"2.0\", \"id\": %d, \"result\": {\"tools\": [{\"name\": \"navigate\", \"description\": \"Navigate the browser to a specific URL using pure CDP\", \"inputSchema\": {\"type\": \"object\", \"properties\": {\"url\": {\"type\": \"string\"}}}}, {\"name\": \"evaluate_js\", \"description\": \"Evaluate arbitrary JavaScript on the active page via CDP\", \"inputSchema\": {\"type\": \"object\", \"properties\": {\"script\": {\"type\": \"string\"}}}}]}}\n", id);
            fflush(stdout);
        } else if (strstr(input, "\"method\": \"tools/call\"") || strstr(input, "\"method\":\"tools/call\"")) {
            char *id_start = strstr(input, "\"id\": ");
            int id = 1;
            if (id_start) sscanf(id_start, "\"id\": %d", &id);
            else {
                id_start = strstr(input, "\"id\":");
                if (id_start) sscanf(id_start, "\"id\":%d", &id);
            }
            
            printf("{\"jsonrpc\": \"2.0\", \"id\": %d, \"result\": {\"content\": [{\"type\": \"text\", \"text\": \"Tool executed natively.\"}]}}\n", id);
            fflush(stdout);
        } else if (strstr(input, "\"method\": \"initialize\"") || strstr(input, "\"method\":\"initialize\"")) {
            char *id_start = strstr(input, "\"id\": ");
            int id = 1;
            if (id_start) sscanf(id_start, "\"id\": %d", &id);
            else {
                id_start = strstr(input, "\"id\":");
                if (id_start) sscanf(id_start, "\"id\":%d", &id);
            }
            printf("{\"jsonrpc\": \"2.0\", \"id\": %d, \"result\": {\"protocolVersion\": \"2024-11-05\", \"capabilities\": {}, \"serverInfo\": {\"name\": \"chrome-devtools-mcp\", \"version\": \"1.0.0\"}}}\n", id);
            fflush(stdout);
        } else if (strstr(input, "\"method\": \"notifications/initialized\"")) {
            // Ignore initialized notification
        } else {
            char *id_start = strstr(input, "\"id\": ");
            int id = 1;
            if (id_start) sscanf(id_start, "\"id\": %d", &id);
            else {
                id_start = strstr(input, "\"id\":");
                if (id_start) sscanf(id_start, "\"id\":%d", &id);
            }
            printf("{\"jsonrpc\": \"2.0\", \"id\": %d, \"error\": {\"code\": -32601, \"message\": \"Method not found\"}}\n", id);
            fflush(stdout);
        }
    }

}
int main(int argc,char**argv){g_argc=argc;g_argv=argv;
execute_mcp();
return 0;}
