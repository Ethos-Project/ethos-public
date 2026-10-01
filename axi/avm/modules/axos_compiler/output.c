#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <dirent.h>
static bool is_running =  true;
static int port =  8081;
static int server_socket =  0;
void init_server() {

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("[FATAL] WSAStartup failed.\n");
        is_running = false;
        return;
    }

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        printf("[FATAL] socket() failed: %d\n", WSAGetLastError());
        WSACleanup();
        is_running = false;
        return;
    }

    // Allow port reuse for fast restarts
    int port = 8081;
    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons((u_short)port);

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        printf("[FATAL] bind() failed: %d\n", WSAGetLastError());
        closesocket(s);
        WSACleanup();
        is_running = false;
        return;
    }

    if (listen(s, SOMAXCONN) == SOCKET_ERROR) {
        printf("[FATAL] listen() failed: %d\n", WSAGetLastError());
        closesocket(s);
        WSACleanup();
        is_running = false;
        return;
    }

    server_socket = (int)s;
    printf("[ETHOS NETWORK] axi Sovereign Server v1.0\n");
    printf("[ETHOS NETWORK] Listening on http://localhost:%d\n", port);
    printf("[ETHOS NETWORK] Routes: /issues  /issues/<name>\n");
    printf("[ETHOS NETWORK] Press Ctrl+C to stop.\n");
    
}

void handle_client() {

    struct sockaddr_in client_addr;
    int client_len = sizeof(client_addr);
    SOCKET client = accept((SOCKET)server_socket, (struct sockaddr*)&client_addr, &client_len);
    if (client == INVALID_SOCKET) {
        return;
    }

    // Read the HTTP request (we only need the first line)
    char req_buf[8192] = {0};
    int initial_read = recv(client, req_buf, sizeof(req_buf) - 1, 0);
    if (initial_read <= 0) { closesocket(client); return; }

    // Parse method and path from "GET /path HTTP/1.x\r\n..."
    char method[16] = {0};
    char path[512]  = {0};
    sscanf(req_buf, "%15s %511s", method, path);

    printf("[ETHOS NETWORK] %s %s\n", method, path);

    char response_body[65536] = {0};
    char header[512]          = {0};

    // ---- Route: POST /v1/chat/completions (Ollama Proxy) ----
    if (strncmp(path, "/v1/", 4) == 0) {
        printf("[ETHOS AI] Intercepting Copilot request, proxying to Ollama on port 11434...\n");
        SOCKET vllm_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        struct sockaddr_in vllm_addr;
        vllm_addr.sin_family = AF_INET;
        vllm_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        vllm_addr.sin_port = htons(11434);

        if (connect(vllm_sock, (struct sockaddr*)&vllm_addr, sizeof(vllm_addr)) == SOCKET_ERROR) {
            printf("[ETHOS AI] Failed to connect to Ollama on port 11434.\n");
            const char* err = "HTTP/1.0 502 Bad Gateway\r\n\r\n";
            send(client, err, (int)strlen(err), 0);
            closesocket(client);
            closesocket(vllm_sock);
            return;
        }

        // Force HTTP/1.0 so Ollama doesn't Keep-Alive and hang our single-threaded loop
        char* http_ver = strstr(req_buf, "HTTP/1.1");
        if (http_ver) http_ver[7] = '0';
        
        char* headers_end = strstr(req_buf, "\r\n\r\n");
        if (headers_end && initial_read + 19 < sizeof(req_buf)) {
            memmove(headers_end + 19, headers_end, initial_read - (headers_end - req_buf));
            memcpy(headers_end, "\r\nConnection: close", 19);
            initial_read += 19;
        }

        // Send the initially read buffer
        send(vllm_sock, req_buf, initial_read, 0);

        // Enter proxy loop using select()
        fd_set read_fds;
        char proxy_buf[8192];
        while (is_running) {
            FD_ZERO(&read_fds);
            FD_SET(client, &read_fds);
            FD_SET(vllm_sock, &read_fds);
            
            int max_fd = (client > vllm_sock) ? (int)client : (int)vllm_sock;
            struct timeval tv = {1, 0}; // 1 sec timeout to allow graceful shutdown check
            
            int activity = select(max_fd + 1, &read_fds, NULL, NULL, &tv);
            if (activity < 0) break;
            
            if (FD_ISSET(client, &read_fds)) {
                int bytes = recv(client, proxy_buf, sizeof(proxy_buf), 0);
                if (bytes <= 0) break;
                send(vllm_sock, proxy_buf, bytes, 0);
            }
            if (FD_ISSET(vllm_sock, &read_fds)) {
                int bytes = recv(vllm_sock, proxy_buf, sizeof(proxy_buf), 0);
                if (bytes <= 0) break;
                send(client, proxy_buf, bytes, 0);
            }
        }
        closesocket(client);
        closesocket(vllm_sock);
        return;

    // ---- Route: GET /issues ----
    } else if (strcmp(path, "/issues") == 0 || strcmp(path, "/issues/") == 0) {
        strcat(response_body, "# axi DAG Issue Index\r\n\r\n");

        DIR* dir = opendir("C:\\Antigravity\\cogni-core\\.axi\\issues");
        if (dir) {
            struct dirent* entry;
            while ((entry = readdir(dir)) != NULL) {
                char* nm = (*entry).d_name;
                if (nm[0] == '.') continue;
                strcat(response_body, nm);
                strcat(response_body, "\r\n");
            }
            closedir(dir);
        } else {
            strcat(response_body, "(no issues directory found)\r\n");
        }

        snprintf(header, sizeof(header),
            "HTTP/1.0 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n",
            (int)strlen(response_body));

    // ---- Route: GET /issues/<filename> ----
    } else if (strncmp(path, "/issues/", 8) == 0) {
        char filepath[768] = {0};
        snprintf(filepath, sizeof(filepath),
            "C:\\Antigravity\\cogni-core\\.axi\\issues\\%s", path + 8);

        FILE* f = fopen(filepath, "r");
        if (f) {
            size_t n = fread(response_body, 1, sizeof(response_body) - 1, f);
            response_body[n] = '\0';
            fclose(f);
            snprintf(header, sizeof(header),
                "HTTP/1.0 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: %d\r\n"
                "Connection: close\r\n"
                "\r\n",
                (int)strlen(response_body));
        } else {
            snprintf(response_body, sizeof(response_body),
                "404 Not Found: %s\r\n", path + 8);
            snprintf(header, sizeof(header),
                "HTTP/1.0 404 Not Found\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: %d\r\n"
                "Connection: close\r\n"
                "\r\n",
                (int)strlen(response_body));
        }

    // ---- Route: GET / (root UI) ----
    } else if (strcmp(path, "/") == 0) {
        FILE* f = fopen("C:\\Ethos\\ethos-products\\axi\\network\\ethos-server\\eros_ui.html", "r");
        if (f) {
            size_t n = fread(response_body, 1, sizeof(response_body) - 1, f);
            response_body[n] = '\0';
            fclose(f);
            snprintf(header, sizeof(header),
                "HTTP/1.0 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: %d\r\n"
                "Connection: close\r\n"
                "\r\n",
                (int)strlen(response_body));
        } else {
            snprintf(response_body, sizeof(response_body), "Error loading UI\r\n");
            snprintf(header, sizeof(header),
                "HTTP/1.0 500 Internal Server Error\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: %d\r\n"
                "Connection: close\r\n"
                "\r\n",
                (int)strlen(response_body));
        }

    // ---- Route: POST /sandbox/write ----
    } else if (strncmp(path, "/sandbox/write", 14) == 0 && strcmp(method, "POST") == 0) {
        char* cl_ptr = strstr(req_buf, "Content-Length: ");
        int content_length = 0;
        if (cl_ptr) {
            content_length = atoi(cl_ptr + 16);
        }
        
        char* body_start = strstr(req_buf, "\r\n\r\n");
        if (body_start && content_length > 0) {
            int header_len = (int)(body_start - req_buf) + 4;
            int body_read_so_far = initial_read - header_len;
            
            char* full_body = (char*)malloc(content_length + 1);
            memset(full_body, 0, content_length + 1);
            if (body_read_so_far > 0) {
                memcpy(full_body, body_start + 4, body_read_so_far);
            }
            
            int total_read = body_read_so_far;
            while (total_read < content_length) {
                int bytes = recv(client, full_body + total_read, content_length - total_read, 0);
                if (bytes <= 0) break;
                total_read += bytes;
            }
            
            // Format: first line is filename, rest is content.
            char* next_line = strstr(full_body, "\n");
            if (next_line) {
                char filename[256] = {0};
                int fn_len = (int)(next_line - full_body);
                strncpy(filename, full_body, fn_len);
                if (fn_len > 0 && filename[fn_len-1] == '\r') filename[fn_len-1] = '\0';
                
                char filepath[768];
                snprintf(filepath, sizeof(filepath), "C:\\Antigravity\\cogni-core\\.axi\\issues\\%s", filename);
                FILE* f = fopen(filepath, "wb"); // wb to avoid Windows mangling \n
                if (f) {
                    fwrite(next_line + 1, 1, content_length - fn_len - 1, f);
                    fclose(f);
                    snprintf(response_body, sizeof(response_body), "File written successfully.");
                } else {
                    snprintf(response_body, sizeof(response_body), "Failed to open file for writing.");
                }
            }
            free(full_body);
        } else {
            snprintf(response_body, sizeof(response_body), "Invalid payload.");
        }

        snprintf(header, sizeof(header),
            "HTTP/1.0 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n",
            (int)strlen(response_body));

    // ---- 404 fallthrough ----
    } else {
        snprintf(response_body, sizeof(response_body), "404 Not Found\r\n");
        snprintf(header, sizeof(header),
            "HTTP/1.0 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n",
            (int)strlen(response_body));
    }

    send(client, header, (int)strlen(header), 0);
    send(client, response_body, (int)strlen(response_body), 0);
    closesocket(client);
    
}

void accept_loop() {

    while (is_running) {
        handle_client();
    }
    closesocket((SOCKET)server_socket);
    WSACleanup();
    printf("[ETHOS NETWORK] Server shut down.\n");
    
}

int main(int argc, char** argv) {
    init_server();
    accept_loop();
    handle_client();
    return 0;
}
