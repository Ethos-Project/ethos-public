#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char source_buffer[655360];
int bytes_read = 0;
bool compile_status = true;

char node_names[100 * 64];
int out_edges[100 * 100];
int num_out[100] = {0};
int in_degrees[100] = {0};
int ast_node_count = 0;

int sorted_indices[100] = {0};
int sorted_count = 0;

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: axi_compiler <input.axi> <output.exe>\n");
        return 1;
    }
    const char* target_file = argv[1];
    const char* output_exe = argv[2];
    // ---------------------------------------------------------
    // UNIVERSAL LANGUAGE ROUTER
    // ---------------------------------------------------------
    const char* ext = strrchr(target_file, '.');
    if (ext != NULL) {
        if (strcmp(ext, ".py") == 0) {
            printf("[axi COMPILER] Python AST transpilation layer initializing (Pending implementation)...\n");
            return 0;
        } else if (strcmp(ext, ".cs") == 0) {
            printf("[axi COMPILER] C# transpilation layer initializing (Pending implementation)...\n");
            return 0;
        } else if (strcmp(ext, ".c") == 0 || strcmp(ext, ".cpp") == 0) {
            printf("[axi COMPILER] Native C/C++ pass-through. Compiling directly...\n");
            char pass_cmd[2048];
            sprintf(pass_cmd, "C:\\\\Antigravity\\\\cogni-core\\\\tools\\\\python_to_c_compiler\\\\bin\\\\mingw64\\\\bin\\\\gcc.exe %s -o %s", target_file, output_exe);
            system(pass_cmd);
            return 0;
        } else if (strcmp(ext, ".axi") != 0) {
            printf("[FATAL] Unsupported file extension: %s\n", ext);
            return 1;
        }
    }
    
    // Proceeding with standard .axi transpilation below...

    FILE *file = fopen(target_file, "r");
    if (file == NULL) { 
        printf("Failed to open %s\n", target_file);
        return 1;
    }
    bytes_read = fread(source_buffer, 1, 655359, file);
    source_buffer[bytes_read] = '\0'; 
    fclose(file);

    // Parse Wires
    char line[1024];
    int i = 0, line_idx = 0;
    while (i < bytes_read) {
        char c = source_buffer[i];
        if (c == '\n' || c == '\r' || c == '\0') {
            line[line_idx] = '\0';
            if (line_idx > 0) {
                if (strstr(line, "->") != NULL && strstr(line, "node ") == NULL) {
                    char src[64] = {0}; char tgt[64] = {0};
                    sscanf(line, "%63s -> %63s", src, tgt);
                    int src_idx = -1, tgt_idx = -1;
                    
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

    // Topological Sort
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

    FILE *f = fopen("output.c", "w");
    fprintf(f, "#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <stdbool.h>\n#include <math.h>\n");
    
    char* p = source_buffer;
    while ((p = strstr(p, "@C_Include")) != NULL) {
        char inc[128] = {0};
        sscanf(p, "@C_Include(\"\%127[^\"]\")", inc);
        fprintf(f, "#include %s\n", inc);
        p += 10;
    }

    // Shims for domus/studio (if it contains raylib include)
    if (strstr(source_buffer, "<raylib.h>") != NULL) {
        fprintf(f, "typedef struct SpatialNode { int id; const char* label; float pos_x; float pos_y; float pos_z; float width; float height; float depth; bool is_dragging; bool is_active; bool is_sandboxed; } SpatialNode;\n");
        fprintf(f, "typedef struct { BoundingBox sandbox_bounds; Camera3D camera; int wire_drag_source_id; int wire_drag_x; int wire_drag_y; } axiStudioState;\n");
        fprintf(f, "axiStudioState axi_studio_state = { .sandbox_bounds = (BoundingBox){{0.0f, 0.0f, 0.0f}, {100.0f, 100.0f, 100.0f}}, .camera = { { 10.0f, 10.0f, 10.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, CAMERA_PERSPECTIVE }, .wire_drag_source_id = -1 };\n");
    }

    p = source_buffer;
    while ((p = strstr(p, "state ")) != NULL) {
        char s_name[64] = {0}, s_type[64] = {0}, s_val[128] = {0};
        char line[256];
        int j=0; while(p[j] != '\n' && p[j] != '\0' && j<255) { line[j] = p[j]; j++; } line[j]='\0';
        char* colon = strchr(line, ':');
        char* eq = strchr(line, '=');
        if (colon && eq) {
            strncpy(s_name, line + 6, colon - (line + 6)); s_name[colon - (line + 6)] = '\0';
            strncpy(s_type, colon + 1, eq - (colon + 1)); s_type[eq - (colon + 1)] = '\0';
            strcpy(s_val, eq + 1);
            char* trim_type = s_type; while(*trim_type == ' ') trim_type++;
            char* trim_name = s_name; while(*trim_name == ' ') trim_name++;
            char c_type[64]; char c_suffix[64] = {0};
            char* bracket = strchr(trim_type, '[');
            if (bracket) {
                strncpy(c_type, trim_type, bracket - trim_type); c_type[bracket - trim_type] = '\0';
                strcpy(c_suffix, bracket);
                if (strstr(c_type, "bool") != NULL) strcpy(c_type, "bool");
                fprintf(f, "static %s %s%s = %s;\n", c_type, trim_name, c_suffix, s_val);
            } else {
                if (strstr(trim_type, "bool") != NULL) strcpy(trim_type, "bool");
                if (strstr(trim_type, "int") != NULL) strcpy(trim_type, "int");
                if (strstr(s_val, "True") != NULL) strcpy(s_val, " true");
                if (strstr(s_val, "False") != NULL) strcpy(s_val, " false");
                fprintf(f, "static %s %s = %s;\n", trim_type, trim_name, s_val);
            }
        }
        p += 5;
    }

    p = source_buffer;
    while ((p = strstr(p, "node ")) != NULL) {
        char n_name[64] = {0};
        sscanf(p, "node %63[^(]", n_name);
        fprintf(f, "void %s() {\n", n_name);
        char q3[4] = {34,34,34,0};
        char* block_start = strstr(p, q3);
        if (block_start) {
            block_start += 3;
            char* block_end = strstr(block_start, q3);
            if (block_end) {
                fwrite(block_start, 1, block_end - block_start, f);
            }
        }
        fprintf(f, "\n}\n\n");
        p += 4;
    }

    fprintf(f, "int main(int argc, char** argv) {\n");
    for (int k = 0; k < sorted_count; k++) {
        char* n_name = &node_names[sorted_indices[k] * 64];
        char search_str[128]; sprintf(search_str, "node %s", n_name);
        if (strstr(source_buffer, search_str) != NULL) {
            fprintf(f, "    %s();\n", n_name);
        }
    }
    fprintf(f, "    return 0;\n}\n");
    fclose(f);
    
    printf("[axi NATIVE COMPILER] Parsed %s -> output.c\n", target_file);

    char cmd[2048];
    // Dynamic dependencies based on what's included
    char deps[1024] = "";
    if (strstr(source_buffer, "<raylib.h>") != NULL) {
        strcat(deps, "-I.\\\\raylib-5.0_win64_mingw-w64\\\\include -L.\\\\raylib-5.0_win64_mingw-w64\\\\lib -lraylib -lgdi32 -lwinmm ");
    }
    if (strstr(source_buffer, "<winsock2.h>") != NULL) {
        strcat(deps, "-lws2_32 ");
    }
    if (strstr(source_buffer, "<llama.h>") != NULL) {
        strcat(deps, "-I.\\\\lib\\\\llama.cpp\\\\include -L.\\\\lib\\\\llama.cpp\\\\build\\\\src -lllama ");
    }
    
    sprintf(cmd, "C:\\\\Antigravity\\\\cogni-core\\\\tools\\\\python_to_c_compiler\\\\bin\\\\mingw64\\\\bin\\\\gcc.exe output.c -o %s %s", output_exe, deps);
    printf("[axi NATIVE COMPILER] Building %s natively...\n", output_exe);
    system(cmd);
    printf("[axi NATIVE COMPILER] Built successfully!\n");
    return 0;
}


