@C_Native("
extern int g_argc;
extern char** g_argv;
")
@C_Native("\nextern int g_argc;\nextern char** g_argv;\n")
@C_Include("<stdio.h>")
@C_Include("<stdlib.h>")
@C_Include("<string.h>")

@StaticArena
state source_buffer: char[655360] = {0} 
@StaticArena
state bytes_read: int = 0
@StaticArena
state compile_status: bool = True

@StaticArena
state node_names: char[6400] = {0}
@StaticArena
state in_degrees: int[100] = {0}
@StaticArena
state out_edges: int[10000] = {0}
@StaticArena
state num_out: int[100] = {0}
@StaticArena
state ast_node_count: int = 0
@StaticArena
state sorted_indices: int[100] = {0}
@StaticArena
state sorted_count: int = 0

node load_source_file() -> int:
    @C_Native
    """
    // In a real compiler, we read argv[1]. For bootstrap, hardcode target.
    if (g_argc < 3) { printf("Usage: axi_compiler <input.axi> <output.exe>\n"); compile_status = false; return; }
    const char* target_file = g_argv[1];
    FILE *file = fopen(target_file, "r");
    if (file == NULL) { 
        printf("Failed to open %s\n", target_file);
        compile_status = false; 
        return; 
    }
    bytes_read = fread(source_buffer, 1, 655359, file);
    source_buffer[bytes_read] = '\0'; 
    fclose(file);
    """

node parse_and_sort() -> int:
    @C_Native
    """
    if (!compile_status) return;
    
    // We will do a line-by-line parsing strategy for simplicity
    char line[1024];
    int i = 0, line_idx = 0;
    while (i < bytes_read) {
        char c = source_buffer[i];
        if (c == '\n' || c == '\r' || c == '\0') {
            line[line_idx] = '\0';
            if (line_idx > 0) {
                // Parse Wires
                if ((strstr(line, "->") != NULL || strstr(line, "=>") != NULL) && strstr(line, "node") == NULL && strstr(line, "function") == NULL) {
                    char src[64] = {0}; char tgt[64] = {0};
                    if (strstr(line, "->") != NULL) sscanf(line, "%63s -> %63s", src, tgt);
                    else sscanf(line, "%63s => %63s", src, tgt);
                    int src_idx = -1, tgt_idx = -1;
                    
                    // Auto-register nodes if not seen (handles states & nodes)
                    for(int n = 0; n < ast_node_count; n++) {
                        if(strcmp(&node_names[n*64], src) == 0) src_idx = n;
                        if(strcmp(&node_names[n*64], tgt) == 0) tgt_idx = n;
                    }
                    if(src_idx == -1) { strcpy(&node_names[ast_node_count*64], src); src_idx = ast_node_count++; }
                    if(tgt_idx == -1) { strcpy(&node_names[ast_node_count*64], tgt); tgt_idx = ast_node_count++; }
                    
                    out_edges[src_idx * 100 + num_out[src_idx]] = tgt_idx;
                    num_out[src_idx]++; in_degrees[tgt_idx]++;
                }
            }
            line_idx = 0;
            if (c == '\0') break;
        } else {
            if (line_idx < 1023) line[line_idx++] = c;
        }
        i++;
    }
    
    // KAHN'S SORT
    int queue[100]; int front = 0, back = 0;
    for (int n = 0; n < ast_node_count; n++) { if (in_degrees[n] == 0) queue[back++] = n; }
    while (front < back) {
        int curr = queue[front++];
        sorted_indices[sorted_count++] = curr;
        for (int e = 0; e < num_out[curr]; e++) {
            int tgt = out_edges[curr * 100 + e];
            in_degrees[tgt]--;
            if (in_degrees[tgt] == 0) queue[back++] = tgt;
        }
    }
    """

node emit_c_code() -> int:
    @C_Native
    """
    if (!compile_status) return;
    FILE *f = fopen("output.c", "w");
    
    // 1. Emitting Includes
    fprintf(f, "#include <stdio.h>\\n#include <stdlib.h>\\n#include <string.h>\\n#include <stdbool.h>\\n");
    fprintf(f, "#include <math.h>\\n");
    
    // Extract @C_Include lines
    char state_str[] = {'s','t','a','t','e',' ',0};
    char node_str[] = {'n','o','d','e',' ',0};
    char func_str[] = {'f','u','n','c','t','i','o','n',' ',0};
    char inc_str[] = {'@','C','_','I','n','c','l','u','d','e',0};
    char* p = source_buffer;
    while ((p = strstr(p, inc_str)) != NULL) {
        char inc[128] = {0};
        sscanf(p, "@C_Include(%127[^)])", inc);
        fprintf(f, "#include %s\\n", inc);
        p += 10;
    }
    
    // Raylib Shims
    fprintf(f, "typedef struct SpatialNode { int id; const char* label; float pos_x; float pos_y; float pos_z; float width; float height; float depth; bool is_dragging; bool is_active; bool is_sandboxed; } SpatialNode;\\n");
    fprintf(f, "typedef struct { BoundingBox sandbox_bounds; Camera3D camera; int wire_drag_source_id; int wire_drag_x; int wire_drag_y; } axiStudioState;\\n");
    fprintf(f, "axiStudioState axi_studio_state = { .sandbox_bounds = (BoundingBox){{0.0f, 0.0f, 0.0f}, {100.0f, 100.0f, 100.0f}}, .camera = { { 10.0f, 10.0f, 10.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, CAMERA_PERSPECTIVE }, .wire_drag_source_id = -1 };\\n");
    
    // 2. Emitting States
    p = source_buffer;
    while ((p = strstr(p, state_str)) != NULL) {
        char s_name[64] = {0};
        char s_type[64] = {0};
        char s_val[128] = {0};
        
        // Match: state x: float = 0.0 or state x: int[10] = {0}
        char line[256];
        int i=0; while(p[i] != '\\n' && p[i] != '\\0' && i<255) { line[i] = p[i]; i++; } line[i]='\\0';
        
        char* colon = strchr(line, ':');
        char* eq = strchr(line, '=');
        if (colon && eq) {
            strncpy(s_name, line + 6, colon - (line + 6)); s_name[colon - (line + 6)] = '\\0';
            strncpy(s_type, colon + 1, eq - (colon + 1)); s_type[eq - (colon + 1)] = '\\0';
            strcpy(s_val, eq + 1);
            
            // Trim whitespace
            char* trim_type = s_type; while(*trim_type == ' ') trim_type++;
            char* trim_name = s_name; while(*trim_name == ' ') trim_name++;
            
            // Handle arrays (int[480000])
            char c_type[64]; char c_suffix[64] = {0};
            char* bracket = strchr(trim_type, '[');
            if (bracket) {
                strncpy(c_type, trim_type, bracket - trim_type); c_type[bracket - trim_type] = '\\0';
                strcpy(c_suffix, bracket);
                
                // Map boolean to bool
                if (strstr(c_type, "bool") != NULL) strcpy(c_type, "bool");
                fprintf(f, "static %s %s%s = %s;\\n", c_type, trim_name, c_suffix, s_val);
            } else {
                if (strstr(trim_type, "bool") != NULL) strcpy(trim_type, "bool");
                if (strstr(trim_type, "int") != NULL) strcpy(trim_type, "int");
                
                // Map True/False to true/false
                if (strstr(s_val, "True") != NULL) strcpy(s_val, " true");
                if (strstr(s_val, "False") != NULL) strcpy(s_val, " false");
                
                fprintf(f, "static %s %s = %s;\\n", trim_type, trim_name, s_val);
            }
        }
        p += 5;
    }
    
    // 3. Emitting Nodes & C_Native bodies
    p = source_buffer;
    while (1) {
        char* c1 = strstr(p, node_str);
        char* c2 = strstr(p, func_str);
        char* match = (c1 && c2) ? (c1 < c2 ? c1 : c2) : (c1 ? c1 : c2);
        if (!match) break;
        p = match;
        
        char n_name[64] = {0};
        if (match == c1) sscanf(p, "node %63[^(]", n_name);
        else sscanf(p, "function %63[^(]", n_name);
        
        fprintf(f, "void %s() {\\n", n_name);
        
        // Find string block
        char q3[4] = {34,34,34,0};
        char q1[2] = {'`',0};
        char* b1 = strstr(p, q3);
        char* b2 = strstr(p, q1);
        char* block_start = (b1 && b2) ? (b1 < b2 ? b1 : b2) : (b1 ? b1 : b2);
        if (block_start) {
            int is_q3 = (block_start == b1);
            block_start += is_q3 ? 3 : 1;
            char* block_end = strstr(block_start, is_q3 ? q3 : q1);
            if (block_end) {
                // Print the raw C code exactly as it is!
                fwrite(block_start, 1, block_end - block_start, f);
            }
        }
        fprintf(f, "\\n}\\n\\n");
        p += 4;
    }
    
    // 4. Emitting main() using Topological Sort
    fprintf(f, "int g_argc; char** g_argv;\\nint main(int argc, char** argv) {\\n    g_argc = argc; g_argv = argv;\\n");
    for (int i = 0; i < sorted_count; i++) {
        char* n_name = &node_names[sorted_indices[i] * 64];
        // Only call if it's actually a node (nodes have bodies, states do not)
        char search_str1[128]; sprintf(search_str1, "node %s", n_name);
        char search_str2[128]; sprintf(search_str2, "function %s", n_name);
        if (strstr(source_buffer, search_str1) != NULL || strstr(source_buffer, search_str2) != NULL) {
            fprintf(f, "    %s();\\n", n_name);
        }
    }
    fprintf(f, "    return 0;\\n}\\n");
    fclose(f);
    printf("[axi_COMPILER] Parsed input file and generated output.c\\n");
    """

node compile_binary() -> int:
    @C_Native
    """
    if (!compile_status) return;
    printf("[axi_COMPILER] Invoking MinGW GCC natively to build executable...\\n");
    char cmd[2048];
    sprintf(cmd, "C:\\\\\\\\Antigravity\\\\\\\\cogni-core\\\\\\\\tools\\\\\\\\python_to_c_compiler\\\\\\\\bin\\\\\\\\mingw64\\\\\\\\bin\\\\\\\\gcc.exe output.c -o %s -I.\\\\\\\\raylib-5.0_win64_mingw-w64\\\\\\\\include -L.\\\\\\\\raylib-5.0_win64_mingw-w64\\\\\\\\lib -I.\\\\\\\\lib\\\\\\\\llama.cpp\\\\\\\\include -L.\\\\\\\\lib\\\\\\\\llama.cpp\\\\\\\\build\\\\\\\\src -lraylib -lgdi32 -lwinmm -lllama -lws2_32", g_argv[2]);
    system(cmd);
    printf("[axi_COMPILER] FULL BOOTSTRAP COMPLETE! Executable generated.\\n");
    """

compile_status -> load_source_file
compile_status -> parse_and_sort
compile_status -> emit_c_code
compile_status -> compile_binary
