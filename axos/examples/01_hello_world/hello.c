#include <stdio.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int main(int argc, char** argv) {
    bool active = true;
    while (active) {
        printf("Hello from axos!\n");
        active = false;
    }
    return 0;
}