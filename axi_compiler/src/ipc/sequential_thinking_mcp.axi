node execute_mcp {
`
    char input[4096];
    while (fgets(input, sizeof(input), stdin)) {
        if (strlen(input) <= 1) continue;
        
        if (strstr(input, "\"method\": \"tools/list\"") || strstr(input, "\"method\":\"tools/list\"")) {
            char *id_start = strstr(input, "\"id\": ");
            int id = 1;
            if (id_start) sscanf(id_start, "\"id\": %d", &id);
            else {
                id_start = strstr(input, "\"id\":");
                if (id_start) sscanf(id_start, "\"id\":%d", &id);
            }
            printf("{\"jsonrpc\": \"2.0\", \"id\": %d, \"result\": {\"tools\": [{\"name\": \"sequential_think\", \"description\": \"Native thinking routine\", \"inputSchema\": {\"type\": \"object\", \"properties\": {\"thought\": {\"type\": \"string\"}}}}]}}\n", id);
            fflush(stdout);
        } else if (strstr(input, "\"method\": \"tools/call\"") || strstr(input, "\"method\":\"tools/call\"")) {
            char *id_start = strstr(input, "\"id\": ");
            int id = 1;
            if (id_start) sscanf(id_start, "\"id\": %d", &id);
            else {
                id_start = strstr(input, "\"id\":");
                if (id_start) sscanf(id_start, "\"id\":%d", &id);
            }
            printf("{\"jsonrpc\": \"2.0\", \"id\": %d, \"result\": {\"content\": [{\"type\": \"text\", \"text\": \"Thought recorded natively.\"}]}}\n", id);
            fflush(stdout);
        } else if (strstr(input, "\"method\": \"initialize\"") || strstr(input, "\"method\":\"initialize\"")) {
            char *id_start = strstr(input, "\"id\": ");
            int id = 1;
            if (id_start) sscanf(id_start, "\"id\": %d", &id);
            else {
                id_start = strstr(input, "\"id\":");
                if (id_start) sscanf(id_start, "\"id\":%d", &id);
            }
            printf("{\"jsonrpc\": \"2.0\", \"id\": %d, \"result\": {\"protocolVersion\": \"2024-11-05\", \"capabilities\": {}, \"serverInfo\": {\"name\": \"sequential-thinking-mcp\", \"version\": \"1.0.0\"}}}\n", id);
            fflush(stdout);
        } else if (strstr(input, "\"method\": \"notifications/initialized\"")) {
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
`
}

start -> execute_mcp
