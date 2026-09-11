@Import_TS("../../src/tui/axi_tui")

node init_terminal {
`
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    GetConsoleMode(hOut, &dwOriginalOutMode);
    DWORD dwRequestedOutModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
    SetConsoleMode(hOut, dwOriginalOutMode | dwRequestedOutModes);

    printf("\x1b[?1049h\x1b[?25l");
    fflush(stdout);
`
}

node draw_ui {
`
    printf("\x1b[2J");
    int width = 70;
    int height = 20;
    
    printf("\x1b[2;5H\x1b[38;2;255;165;0m\x1b[48;2;20;20;20m");
    printf("\xda");
    for(int i=0; i<width; i++) printf("\xc4");
    printf("\xbf");

    for(int y=0; y<height; y++) {
        printf("\x1b[%d;5H\xb3", 3 + y);
        for(int i=0; i<width; i++) printf(" ");
        printf("\xb3");
    }

    printf("\x1b[%d;5H\xc0", 3 + height);
    for(int i=0; i<width; i++) printf("\xc4");
    printf("\xd9");

    printf("\x1b[2;8H\x1b[1m AXI TERMINAL : SKELETON EDITION \x1b[0m\x1b[38;2;255;165;0m\x1b[48;2;20;20;20m");

    printf("\x1b[5;8H\x1b[37mWelcome to the Axi Native Terminal (Public).");
    printf("\x1b[7;8H\x1b[36mCommands:");
    printf("\x1b[8;10H[1] axi init    - Initialize local workspace");
    printf("\x1b[9;10H[2] axi build   - Compile JIT target");
    printf("\x1b[10;10H[3] axi run     - Execute native graph");
    
    printf("\x1b[13;8H\x1b[32mEnvironment:");
    printf("\x1b[14;10HMode:    Redistributable / Developer");
    printf("\x1b[15;10HTarget:  Local Host / File System");

    printf("\x1b[19;8H\x1b[33mPress [ESC] to exit.");
    fflush(stdout);
`
}

node input_loop {
`
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
                if (vkey == VK_ESCAPE) {
                    running = 0;
                    break;
                }
            }
        }
    }

    printf("\x1b[?1049l\x1b[?25h");
    fflush(stdout);
    SetConsoleMode(hOut, dwOriginalOutMode);
    SetConsoleMode(hIn, dwOriginalInMode);
`
}

start -> init_terminal
init_terminal -> draw_ui
draw_ui -> input_loop
