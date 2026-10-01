#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
int g_argc; char **g_argv;
static void emit_axi_marker(void){

    printf("AXI_OK\n");
    
}
int main(int argc,char**argv){g_argc=argc;g_argv=argv;
emit_axi_marker();
return 0;}
