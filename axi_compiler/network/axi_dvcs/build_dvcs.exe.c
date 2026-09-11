#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
int g_argc; char **g_argv;
static void build_dvcs(void){

    printf("[Axi Build] Initiating DVCS CLI Compilation natively through Axi...\n");

    wchar_t command[4096];
    
    // Hardcode the path to the internal GCC backend (no spaces, so no quotes needed for cmd.exe)
    swprintf(command, 4096, L"C:\\Ethos\\ethos-products\\Languages\\axi\\bootstrap\\mingw\\bin\\mingw64\\bin\\gcc.exe");

    // Append the standard C compilation flags
    wcscat(command, L" -std=c11 -Wall -Wextra -pedantic -municode -O2 ");
    
    // Add all the source files in network/axi_dvcs
    wcscat(command, L" \"C:\\Ethos\\ethos-products\\Languages\\axi\\network\\axi_dvcs\\axi.c\" ");
    wcscat(command, L" \"C:\\Ethos\\ethos-products\\Languages\\axi\\network\\axi_dvcs\\axi_store.c\" ");
    wcscat(command, L" \"C:\\Ethos\\ethos-products\\Languages\\axi\\network\\axi_dvcs\\axi_manifest.c\" ");
    wcscat(command, L" \"C:\\Ethos\\ethos-products\\Languages\\axi\\network\\axi_dvcs\\axi_object.c\" ");
    wcscat(command, L" \"C:\\Ethos\\ethos-products\\Languages\\axi\\network\\axi_dvcs\\axi_wrap.c\" ");
    wcscat(command, L" \"C:\\Ethos\\ethos-products\\Languages\\axi\\network\\axi_dvcs\\axi_resume.c\" ");
    wcscat(command, L" \"C:\\Ethos\\ethos-products\\Languages\\axi\\network\\axi_dvcs\\axi_runner.c\" ");
    
    // Add the bcrypt library for cryptography
    wcscat(command, L" -lbcrypt ");

    // Output the fresh axi.exe binary
    wcscat(command, L" -o \"C:\\Ethos\\ethos-products\\Languages\\axi\\network\\axi_dvcs\\axi.exe\" ");
    
    // Execute the compiler
    int result = _wsystem(command);

    if (result == 0) {
        printf("[Axi Build] SUCCESS: Fresh axi.exe compiled and ready for native execution.\n");
    } else {
        printf("[Axi Build] FATAL ERROR: Compilation failed with code %d.\n", result);
    }

}
static void start(void){

    // Kick off the build pipeline

}
int main(int argc,char**argv){g_argc=argc;g_argv=argv;
start();
build_dvcs();
return 0;}
