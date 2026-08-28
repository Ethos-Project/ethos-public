#pragma once

#ifdef _WIN32
    #define AXIOM_EXPORT __declspec(dllexport)
#else
    #define AXIOM_EXPORT __attribute__((visibility("default")))
#endif

extern "C" {

struct RenderCommand {
    int type; // 0=Rect, 1=Text, 2=Grid, 3=Line
    float x, y, w, h; // Grid: x,y = cam offset
    float r, g, b, a;
    char text[64];
};

AXIOM_EXPORT void Axiom_Initialize();
AXIOM_EXPORT int Axiom_ParseSource(const char* sourceCode);
AXIOM_EXPORT const char* Axiom_GetLastError();

AXIOM_EXPORT void axiom_init_window(int width, int height);
AXIOM_EXPORT int axiom_render_frame(double deltaTime, RenderCommand* outCommands, int maxCommands);
AXIOM_EXPORT void axiom_handle_input(int key, int state);

// Mouse Interop
// button: 0=Left, 1=Right, 2=Middle. state: 1=Down, 0=Up
AXIOM_EXPORT void axiom_handle_mouse(float x, float y, int button, int state);
AXIOM_EXPORT void axiom_handle_cursor(float x, float y);

}
