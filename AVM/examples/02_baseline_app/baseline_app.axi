@C_Include("<stdio.h>")

@StaticArena
state app_name: char[64] = "axi Baseline App"
@StaticArena
state is_running: bool = true

node init() -> int:
    @C_Native
    "\""
    printf("Initializing %s...\n", app_name);
    "\""

node main_loop() -> int:
    @C_Native
    "\""
    // Baseline application loop
    is_running = false;
    "\""

init -> main_loop
