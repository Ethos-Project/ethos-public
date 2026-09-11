@C_Include("<stdio.h>")
@C_Include("<stdlib.h>")
@C_Include("<string.h>")
@C_Include("<direct.h>")
@C_Include("<windows.h>")

node verify_compiler_routes() -> int:
    @C_Native
    """
    const char *compiler = g_argc > 1 ? g_argv[1] : "lang\\axi_compiler.exe";
    const char *root = g_argc > 2 ? g_argv[2] : ".";
    const char *relative_inputs[] = {
        "lang\\tests\\compiler\\fixtures\\hello_graph.axi",
        "lang\\tests\\compiler\\fixtures\\hello.c",
        "lang\\tests\\compiler\\fixtures\\hello.cpp",
        "lang\\tests\\compiler\\fixtures\\hello.cs",
        "lang\\tests\\compiler\\fixtures\\hello.py"
    };
    const char *names[] = {"axi", "c", "cpp", "cs", "py"};
    const char *markers[] = {"AXI_OK", "C_OK", "CPP_OK", "CS_OK", "PY_OK"};
    char command[4096];
    char input[1024];
    char output[1024];
    char capture[1024];
    char line[256];
    int failures = 0;
    int i;

    _mkdir("network\\tmp\\axi-wrap-sdd\\compiler-routes");
    for (i = 0; i < 5; ++i) {
        FILE *stream;
        snprintf(input, sizeof(input), "%s\\%s", root, relative_inputs[i]);
        snprintf(output, sizeof(output),
                 "%s\\network\\tmp\\axi-wrap-sdd\\compiler-routes\\hello-%s.exe",
                 root, names[i]);
        snprintf(capture, sizeof(capture),
                 "%s\\network\\tmp\\axi-wrap-sdd\\compiler-routes\\hello-%s.txt",
                 root, names[i]);
        DeleteFileA(output);
        DeleteFileA(capture);
        snprintf(command, sizeof(command), "\"\"%s\" \"%s\" \"%s\"\"",
                 compiler, input, output);
        if (system(command) != 0 || GetFileAttributesA(output) == INVALID_FILE_ATTRIBUTES) {
            fprintf(stderr, "FAIL route=%s build\n", names[i]);
            ++failures;
            continue;
        }
        snprintf(command, sizeof(command), "\"\"%s\" > \"%s\"\"", output, capture);
        if (system(command) != 0) {
            fprintf(stderr, "FAIL route=%s run\n", names[i]);
            ++failures;
            continue;
        }
        stream = fopen(capture, "rb");
        if (!stream || !fgets(line, sizeof(line), stream)) {
            if (stream) fclose(stream);
            fprintf(stderr, "FAIL route=%s capture\n", names[i]);
            ++failures;
            continue;
        }
        fclose(stream);
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, markers[i]) != 0) {
            fprintf(stderr, "FAIL route=%s marker=%s\n", names[i], line);
            ++failures;
            continue;
        }
        printf("PASS route=%s marker=%s\n", names[i], line);
    }

    if (failures) exit(1);
    printf("PASS all base axi routes\n");
    """

start -> verify_compiler_routes
