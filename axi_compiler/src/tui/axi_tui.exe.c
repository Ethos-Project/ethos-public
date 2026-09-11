#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
// Axi JS/TS Transpilation Link: axi_tui
#include "axi_tui.h"
int g_argc; char **g_argv;
static void init_terminal(void){


    // Enable VT100 ANSI escape sequences on Windows
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    GetConsoleMode(hOut, &dwOriginalOutMode);
    DWORD dwRequestedOutModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
    SetConsoleMode(hOut, dwOriginalOutMode | dwRequestedOutModes);

    // Enter alternate screen buffer and hide cursor
    printf("\x1b[?1049h\x1b[?25l");
    fflush(stdout);

}
static void draw_ui(void){

    // Clear screen
    printf("\x1b[2J");

    // Draw a box
    int width = 60;
    int height = 15;
    
    // Top border
    printf("\x1b[2;5H\x1b[38;2;0;255;128m\x1b[48;2;30;30;30m");
    printf("\xda");
    for(int i=0; i<width; i++) printf("\xc4");
    printf("\xbf");

    // Sides
    for(int y=0; y<height; y++) {
        printf("\x1b[%d;5H\xb3", 3 + y);
        for(int i=0; i<width; i++) printf(" ");
        printf("\xb3");
    }

    // Bottom border
    printf("\x1b[%d;5H\xc0", 3 + height);
    for(int i=0; i<width; i++) printf("\xc4");
    printf("\xd9");

    // Title
    printf("\x1b[2;8H\x1b[1m AXI TUI NATIVE ENGINE \x1b[0m\x1b[38;2;0;255;128m\x1b[48;2;30;30;30m");

    // Content
    printf("\x1b[5;8H\x1b[37mWelcome to the native Axi TUI Library.");
    printf("\x1b[7;8H\x1b[36mFeatures:");
    printf("\x1b[8;10H- 24-bit RGB Color Rendering");
    printf("\x1b[9;10H- VT100 Escape Sequences");
    printf("\x1b[10;10H- Zero Node.js Overhead");
    printf("\x1b[11;10H- Fully Native C Execution");

    printf("\x1b[14;8H\x1b[33mPress [ESC] or [ENTER] to exit.");
    fflush(stdout);

}
static void input_loop(void){

    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD dwOriginalInMode;
    GetConsoleMode(hIn, &dwOriginalInMode);
    SetConsoleMode(hIn, ENABLE_VIRTUAL_TERMINAL_INPUT);

    INPUT_RECORD irInBuf[128]; 
    DWORD cNumRead; 
    int running = 1;

    while (running) {
        ReadConsoleInput(hIn, irInBuf, 128, &cNumRead);
        for (DWORD i = 0; i < cNumRead; i++) {
            if (irInBuf[i].EventType == KEY_EVENT && irInBuf[i].Event.KeyEvent.bKeyDown) {
                WORD vkey = irInBuf[i].Event.KeyEvent.wVirtualKeyCode;
                if (vkey == VK_ESCAPE || vkey == VK_RETURN) {
                    running = 0;
                    break;
                }
            }
        }
    }

    // Restore terminal mode and exit alternate screen
    printf("\x1b[?1049l\x1b[?25h");
    fflush(stdout);
    SetConsoleMode(hOut, dwOriginalOutMode);
    SetConsoleMode(hIn, dwOriginalInMode);

}
int main(int argc,char**argv){g_argc=argc;g_argv=argv;
init_terminal();
draw_ui();
input_loop();
return 0;}
