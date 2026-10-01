#pragma once

#ifdef _WIN32
    #define axi_EXPORT __declspec(dllexport)
#else
    #define axi_EXPORT __attribute__((visibility("default")))
#endif

extern "C" {

struct RenderCommand {
    int type; // 0=Rect, 1=Text, 2=Grid, 3=Line
    float x, y, w, h; // Grid: x,y = cam offset
    float r, g, b, a;
    char text[64];
};

axi_EXPORT void axi_Initialize();
axi_EXPORT int axi_ParseSource(const char* sourceCode);
axi_EXPORT const char* axi_GetLastError();

axi_EXPORT void axi_init_window(int width, int height);
axi_EXPORT int axi_render_frame(double deltaTime, RenderCommand* outCommands, int maxCommands);
axi_EXPORT void axi_handle_input(int key, int state);

// Mouse Interop
// button: 0=Left, 1=Right, 2=Middle. state: 1=Down, 0=Up
axi_EXPORT void axi_handle_mouse(float x, float y, int button, int state);
axi_EXPORT void axi_handle_cursor(float x, float y);

}
