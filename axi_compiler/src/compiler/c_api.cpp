#include "c_api.h"
#include "lexer.h"
#include "AST.h"
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>

volatile FFI_SharedMemory* eros_bus = nullptr;

namespace Axi {

void DAG::verifyEulerianCircuits() {
    active_circuits.clear();
    if (!shm_bus) return;

    // FEP Weighting Pass
    for (auto& w : edges) {
        if (!w->sourceNode || !w->targetNode) continue;

        // Weight by prior surprise and precision
        float surprise = euler_get_prior_surprise(shm_bus, w->targetNode->title.c_str());
        float precision = euler_get_precision(shm_bus, w->targetNode->title.c_str());
        w->memory_weight = surprise * precision;
        w->is_euler_circuit = false;
    }

    // Simplified Hierholzer's approximation for demo:
    // Extract closed loops and assign them a memory buffer
    std::unordered_map<std::shared_ptr<Node>, std::vector<std::shared_ptr<Wire>>> adj;
    for (auto& w : edges) {
        adj[w->sourceNode].push_back(w);
    }

    // Sort edges by FEP memory_weight descending
    for (auto& pair : adj) {
        std::sort(pair.second.begin(), pair.second.end(), [](const std::shared_ptr<Wire>& a, const std::shared_ptr<Wire>& b) {
            return a->memory_weight > b->memory_weight;
        });
    }

    // Simple loop detection
    for (auto& start_node : nodes) {
        std::vector<std::shared_ptr<Wire>> path;
        std::shared_ptr<Node> current = start_node;
        bool closed = false;

        while (true) {
            if (adj[current].empty()) break;
            auto next_edge = adj[current].front();

            // Check if already in path (naive)
            bool visited = false;
            for(auto& ew : path) { if(ew == next_edge) visited = true; }
            if (visited) break;

            path.push_back(next_edge);
            current = next_edge->targetNode;

            if (current == start_node) {
                closed = true;
                break;
            }
        }

        if (closed && path.size() > 0) {
            // Validate and allocate circuit!
            Circuit c;
            c.path = path;
            // Allocate 1024 bytes per circuit block
            c.heap_offset = euler_alloc(shm_bus, 1024);

            for (auto& w : path) {
                w->is_euler_circuit = true;
            }
            active_circuits.push_back(c);
        }
    }
}

}

extern "C" {

static Axi::DAG g_dag;
static FFI_SharedMemory g_shm_inst;

void Axi_Initialize() {
    g_dag.nodes.clear();
    g_dag.edges.clear();
    g_dag.active_circuits.clear();

    // Initialize FFI bus
    eros_bus = &g_shm_inst;
    g_dag.shm_bus = (FFI_SharedMemory*)eros_bus;
    euler_pool_init(g_dag.shm_bus);
}

int Axi_ParseSource(const char* sourceCode) { return 0; }
const char* Axi_GetLastError() { return "No error"; }

static int s_width = 1280;
static int s_height = 720;

static float s_camX = 0;
static float s_camY = 0;
static float s_zoom = 1.0f;
static bool s_isPanning = false;
static float s_panStartX = 0;
static float s_panStartY = 0;
static float s_camStartX = 0;
static float s_camStartY = 0;

static int s_draggingNodeIdx = -1;
static float s_dragOffsetX = 0;
static float s_dragOffsetY = 0;

static bool s_showContextMenu = false;
static float s_contextMenuX = 0;
static float s_contextMenuY = 0;
static int s_contextMenuTargetNodeIdx = -1;

// Drawing state
static std::shared_ptr<Axi::Node> s_drawingWireFrom = nullptr;
static float s_mouseX = 0;
static float s_mouseY = 0;

void Axi_init_window(int width, int height) {
    s_width = width;
    s_height = height;

    if (g_dag.nodes.empty()) {
        Axi_Initialize(); // init shm

        auto n1 = std::make_shared<Axi::Node>();
        n1->title = "Trigger Event";
        n1->ui_x = 100; n1->ui_y = 200;

        auto n2 = std::make_shared<Axi::Node>();
        n2->title = "Execute Action";
        n2->ui_x = 400; n2->ui_y = 300;

        g_dag.nodes.push_back(n1);
        g_dag.nodes.push_back(n2);

        auto w = std::make_shared<Axi::Wire>();
        w->sourceNode = n1;
        w->targetNode = n2;
        w->style = Axi::WireStyle::SOLID;
        g_dag.edges.push_back(w);

        g_dag.verifyEulerianCircuits();
    }
}

void SetText(RenderCommand& cmd, const char* str) {
    strncpy(cmd.text, str, 63);
    cmd.text[63] = '\0';
}

int Axi_render_frame(double deltaTime, RenderCommand* outCommands, int maxCommands) {
    int count = 0;
    if (maxCommands < 200) return 0;

    outCommands[count++] = {2, s_camX, s_camY, (float)s_width, (float)s_height, 253.0f/255.0f, 246.0f/255.0f, 238.0f/255.0f, s_zoom};

    for (const auto& edge : g_dag.edges) {
        if (!edge->sourceNode || !edge->targetNode) continue;
        float startX = (edge->sourceNode->ui_x + 200.0f) * s_zoom + s_camX;
        float startY = (edge->sourceNode->ui_y + 60.0f) * s_zoom + s_camY;
        float endX = (edge->targetNode->ui_x) * s_zoom + s_camX;
        float endY = (edge->targetNode->ui_y + 60.0f) * s_zoom + s_camY;

        // Glow green if in Euler Circuit
        if (edge->is_euler_circuit) {
            outCommands[count++] = {3, startX, startY, endX, endY, 0.2f, 0.8f, 0.2f, 1.0f};
        } else {
            outCommands[count++] = {3, startX, startY, endX, endY, 0.2f, 0.2f, 0.2f, 1.0f};
        }
    }

    if (s_drawingWireFrom) {
        float startX = (s_drawingWireFrom->ui_x + 200.0f) * s_zoom + s_camX;
        float startY = (s_drawingWireFrom->ui_y + 60.0f) * s_zoom + s_camY;
        outCommands[count++] = {3, startX, startY, s_mouseX, s_mouseY, 0.8f, 0.4f, 0.2f, 1.0f};
    }

    for (size_t i = 0; i < g_dag.nodes.size(); i++) {
        const auto& n = g_dag.nodes[i];
        float screenX = n->ui_x * s_zoom + s_camX;
        float screenY = n->ui_y * s_zoom + s_camY;
        float scaledW = 200.0f * s_zoom;
        float scaledH = 80.0f * s_zoom;

        // Shadow
        outCommands[count++] = {4, screenX - 6.0f*s_zoom + 3.0f*s_zoom, screenY - 6.0f*s_zoom + 5.0f*s_zoom, 212.0f*s_zoom, 92.0f*s_zoom, 0.0f, 0.0f, 0.0f, 0.15f};

        outCommands[count++] = {0, screenX, screenY, scaledW, scaledH, 1.0f, 1.0f, 1.0f, 1.0f};
        outCommands[count++] = {0, screenX, screenY, scaledW, 25.0f * s_zoom, 0.95f, 0.95f, 0.95f, 1.0f};
        outCommands[count] = {1, screenX + 10.0f * s_zoom, screenY + 5.0f * s_zoom, 14.0f * s_zoom, 0.0f, 44.0f/255.0f, 26.0f/255.0f, 14.0f/255.0f, 1.0f};
        SetText(outCommands[count++], n->title.c_str());

        outCommands[count++] = {5, screenX - 6.0f * s_zoom, screenY + 54.0f * s_zoom, 12.0f * s_zoom, 12.0f * s_zoom, 192.0f/255.0f, 56.0f/255.0f, 90.0f/255.0f, 1.0f};
        outCommands[count] = {1, screenX + 10.0f * s_zoom, screenY + 52.0f * s_zoom, 12.0f * s_zoom, 0.0f, 0.4f, 0.4f, 0.4f, 1.0f};
        SetText(outCommands[count++], "in_1");

        outCommands[count++] = {5, screenX + scaledW - 6.0f * s_zoom, screenY + 54.0f * s_zoom, 12.0f * s_zoom, 12.0f * s_zoom, 192.0f/255.0f, 56.0f/255.0f, 90.0f/255.0f, 1.0f};
        outCommands[count] = {1, screenX + 160.0f * s_zoom, screenY + 52.0f * s_zoom, 12.0f * s_zoom, 0.0f, 0.4f, 0.4f, 0.4f, 1.0f};
        SetText(outCommands[count++], "out_1");
    }

    outCommands[count++] = {0, 0.0f, 0.0f, (float)s_width, 35.0f, 253.0f/255.0f, 246.0f/255.0f, 238.0f/255.0f, 1.0f};
    outCommands[count] = {1, 20.0f, 22.0f, 14.0f, 0.0f, 44.0f/255.0f, 26.0f/255.0f, 14.0f/255.0f, 1.0f};
    SetText(outCommands[count++], "File     View     Editor");

    outCommands[count++] = {0, (float)s_width - 250.0f, 0.0f, 250.0f, 35.0f, 0.9f, 0.9f, 0.9f, 0.8f};
    outCommands[count] = {5, (float)s_width - 240.0f, 11.0f, 12.0f, 12.0f, 0.2f, 0.8f, 0.2f, 1.0f};
    outCommands[count] = {1, (float)s_width - 220.0f, 22.0f, 14.0f, 0.0f, 44.0f/255.0f, 26.0f/255.0f, 14.0f/255.0f, 1.0f};

    char fep_str[64];
    float used = (float)g_dag.shm_bus->heap_head / (float)sizeof(g_dag.shm_bus->dynamic_heap);
    sprintf(fep_str, "FEP: %.4f (Prec: 1.00)", used);
    SetText(outCommands[count++], fep_str);

        if (s_showContextMenu) {
        float menuHeight = (s_contextMenuTargetNodeIdx == -1) ? 100.0f : 130.0f;
        outCommands[count++] = {4, s_contextMenuX - 6.0f + 3.0f, s_contextMenuY - 6.0f + 5.0f, 212.0f, menuHeight + 12.0f, 0.0f, 0.0f, 0.0f, 0.15f};
        outCommands[count++] = {0, s_contextMenuX, s_contextMenuY, 200.0f, menuHeight, 1.0f, 1.0f, 1.0f, 1.0f};

        if (s_contextMenuTargetNodeIdx == -1) {
            outCommands[count] = {1, s_contextMenuX + 10.0f, s_contextMenuY + 15.0f, 14.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
            SetText(outCommands[count++], "Create Node");
            outCommands[count] = {1, s_contextMenuX + 10.0f, s_contextMenuY + 45.0f, 14.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
            SetText(outCommands[count++], "Filter Viewing Mode");
            outCommands[count] = {1, s_contextMenuX + 10.0f, s_contextMenuY + 75.0f, 14.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
            SetText(outCommands[count++], "Multi-Layer Vector Field");
        } else {
            outCommands[count] = {1, s_contextMenuX + 10.0f, s_contextMenuY + 15.0f, 14.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
            SetText(outCommands[count++], "Edit Node");
            outCommands[count] = {1, s_contextMenuX + 10.0f, s_contextMenuY + 45.0f, 14.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
            SetText(outCommands[count++], "Delete Node");
            outCommands[count] = {1, s_contextMenuX + 10.0f, s_contextMenuY + 75.0f, 14.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
            SetText(outCommands[count++], "Adjust Node Properties");
            outCommands[count] = {1, s_contextMenuX + 10.0f, s_contextMenuY + 105.0f, 14.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
            SetText(outCommands[count++], "Activate / Deactivate");
        }
    }

    return count;
}

void Axi_handle_input(int key, int state) {}

void Axi_handle_mouse(float x, float y, int button, int state) {
    s_mouseX = x; s_mouseY = y;

    if (button == 3 && state == 1) {
        if (y > 0) s_zoom *= 1.1f;
        else if (y < 0) s_zoom /= 1.1f;
        if (s_zoom < 0.1f) s_zoom = 0.1f;
        if (s_zoom > 5.0f) s_zoom = 5.0f;
        return;
    }

    if (state == 1) {
        if (button == 0) {
            if (s_showContextMenu) {
                if (x >= s_contextMenuX && x <= s_contextMenuX + 200.0f &&
                    y >= s_contextMenuY && y <= s_contextMenuY + 100.0f) {

                    int clickedIdx = (y - s_contextMenuY) / 30.0f;

                    if (s_contextMenuTargetNodeIdx == -1) {
                        if (clickedIdx == 0) {
                            auto n = std::make_shared<Axi::Node>();
                            n->title = "New Node";
                            n->ui_x = (s_contextMenuX - s_camX) / s_zoom;
                            n->ui_y = (s_contextMenuY - s_camY) / s_zoom;
                            g_dag.nodes.push_back(n);
                            g_dag.verifyEulerianCircuits();
                        }
                    } else {
                        if (clickedIdx == 1) {
                            if (s_contextMenuTargetNodeIdx >= 0 && s_contextMenuTargetNodeIdx < g_dag.nodes.size()) {
                                auto target = g_dag.nodes[s_contextMenuTargetNodeIdx];
                                g_dag.nodes.erase(g_dag.nodes.begin() + s_contextMenuTargetNodeIdx);
                                for (auto it = g_dag.edges.begin(); it != g_dag.edges.end(); ) {
                                    if ((*it)->sourceNode == target || (*it)->targetNode == target) {
                                        it = g_dag.edges.erase(it);
                                    } else {
                                        ++it;
                                    }
                                }
                                g_dag.verifyEulerianCircuits();
                            }
                        }
                    }
                    s_showContextMenu = false;
                    return;
                }
            }

            s_showContextMenu = false;
            float worldX = (x - s_camX) / s_zoom;
            float worldY = (y - s_camY) / s_zoom;

            // Wire creation hit test (right side output pin)
            for (auto& n : g_dag.nodes) {
                float pinX = n->ui_x + 200.0f;
                float pinY = n->ui_y + 60.0f;
                if (abs(worldX - pinX) < 15.0f && abs(worldY - pinY) < 15.0f) {
                    s_drawingWireFrom = n;
                    return;
                }
            }

            s_draggingNodeIdx = -1;
            for (int i = (int)g_dag.nodes.size() - 1; i >= 0; i--) {
                const auto& n = g_dag.nodes[i];
                if (worldX >= n->ui_x && worldX <= n->ui_x + 200.0f &&
                    worldY >= n->ui_y && worldY <= n->ui_y + 80.0f) {
                    s_draggingNodeIdx = i;
                    s_dragOffsetX = worldX - n->ui_x;
                    s_dragOffsetY = worldY - n->ui_y;
                    break;
                }
            }
        } else if (button == 2) {
            s_showContextMenu = false;
            s_isPanning = true;
            s_panStartX = x; s_panStartY = y;
            s_camStartX = s_camX; s_camStartY = s_camY;
        } else if (button == 1) {
            s_showContextMenu = true;
            s_contextMenuX = x;
            s_contextMenuY = y;
            s_contextMenuTargetNodeIdx = -1;
            float worldX = (x - s_camX) / s_zoom;
            float worldY = (y - s_camY) / s_zoom;
            for (int i = (int)g_dag.nodes.size() - 1; i >= 0; i--) {
                const auto& n = g_dag.nodes[i];
                if (worldX >= n->ui_x && worldX <= n->ui_x + 200.0f &&
                    worldY >= n->ui_y && worldY <= n->ui_y + 80.0f) {
                    s_contextMenuTargetNodeIdx = i;
                    break;
                }
            }
        }
    } else {
        if (button == 0 && s_drawingWireFrom) {
            float worldX = (x - s_camX) / s_zoom;
            float worldY = (y - s_camY) / s_zoom;
            for (auto& n : g_dag.nodes) {
                float pinX = n->ui_x;
                float pinY = n->ui_y + 60.0f;
                if (abs(worldX - pinX) < 15.0f && abs(worldY - pinY) < 15.0f && n != s_drawingWireFrom) {
                    auto w = std::make_shared<Axi::Wire>();
                    w->sourceNode = s_drawingWireFrom;
                    w->targetNode = n;
                    w->style = Axi::WireStyle::SOLID;
                    g_dag.edges.push_back(w);
                    g_dag.verifyEulerianCircuits();
                    break;
                }
            }
            s_drawingWireFrom = nullptr;
        }

        if (button == 2) s_isPanning = false;
        if (button == 0) s_draggingNodeIdx = -1;
    }
}

void Axi_handle_cursor(float x, float y) {
    s_mouseX = x; s_mouseY = y;
    if (s_isPanning) {
        s_camX = s_camStartX + (x - s_panStartX);
        s_camY = s_camStartY + (y - s_panStartY);
    } else if (s_draggingNodeIdx >= 0) {
        g_dag.nodes[s_draggingNodeIdx]->ui_x = (x - s_camX) / s_zoom - s_dragOffsetX;
        g_dag.nodes[s_draggingNodeIdx]->ui_y = (y - s_camY) / s_zoom - s_dragOffsetY;
    }
}

}
