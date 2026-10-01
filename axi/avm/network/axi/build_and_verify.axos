@C_Include("<stdio.h>")
@C_Include("<stdlib.h>")
@C_Include("<string.h>")
@C_Include("<ctype.h>")
@C_Include("<direct.h>")
@C_Include("<windows.h>")

node build_and_verify_dvcs() -> int:
    @C_Native
    """
    const char *root = g_argc > 1 ? g_argv[1] : NULL;
    const char *output = g_argc > 2 ? g_argv[2] : NULL;
    char default_output[1024];
    char gcc[1024];
    char command[8192];
    char temp_base[1024];
    char fixture[1200];
    char path[1600];
    char restore[1400];
    char ref[80];
    char payload[16];
    DWORD temp_length;
    FILE *stream;
    size_t count;
    int failures = 0;
    int i;

    if (!root) {
        fprintf(stderr, "Usage: build_and_verify.exe <axi-root> [candidate.exe]\n");
        exit(2);
    }
    if (!output) {
        snprintf(default_output, sizeof(default_output),
                 "%s\\network\\tmp\\axi-wrap-sdd\\axi-rc.exe", root);
        output = default_output;
    }
    snprintf(gcc, sizeof(gcc),
             "%s\\bootstrap\\mingw\\bin\\mingw64\\bin\\gcc.exe", root);
    snprintf(command, sizeof(command),
             "\"\"%s\" -std=c11 -Wall -Wextra -Werror -pedantic -municode "
             "\"%s\\network\\axi_dvcs\\axi.c\" "
             "\"%s\\network\\axi_dvcs\\axi_store.c\" "
             "\"%s\\network\\axi_dvcs\\axi_manifest.c\" "
             "\"%s\\network\\axi_dvcs\\axi_object.c\" "
             "\"%s\\network\\axi_dvcs\\axi_wrap.c\" "
             "\"%s\\network\\axi_dvcs\\axi_resume.c\" "
             "\"%s\\network\\axi_dvcs\\axi_runner.c\" "
             "-o \"%s\" -lbcrypt\"",
             gcc, root, root, root, root, root, root, root, output);
    if (system(command) != 0 || GetFileAttributesA(output) == INVALID_FILE_ATTRIBUTES) {
        fprintf(stderr, "FAIL dvcs strict native build\n");
        exit(1);
    }
    printf("PASS dvcs strict native build\n");

    temp_length = GetTempPathA((DWORD)sizeof(temp_base), temp_base);
    if (!temp_length || temp_length >= sizeof(temp_base)) exit(1);
    snprintf(fixture, sizeof(fixture), "%saxi-dvcs-native-%lu-%llu",
             temp_base, (unsigned long)GetCurrentProcessId(),
             (unsigned long long)GetTickCount64());
    if (!CreateDirectoryA(fixture, NULL)) exit(1);

    snprintf(command, sizeof(command), "\"\"%s\" init --root \"%s\"\"", output, fixture);
    if (system(command) != 0) { fprintf(stderr, "FAIL dvcs init\n"); failures = 1; goto cleanup; }
    snprintf(path, sizeof(path), "%s\\payload.bin", fixture);
    stream = fopen(path, "wb");
    if (!stream || fwrite("DVCS_OK", 1, 7, stream) != 7 || fclose(stream) != 0) {
        if (stream) fclose(stream);
        fprintf(stderr, "FAIL dvcs fixture write\n"); failures = 1; goto cleanup;
    }
    snprintf(path, sizeof(path), "%s\\.axi\\wrap-manifest.toon", fixture);
    stream = fopen(path, "wb");
    if (!stream) { failures = 1; goto cleanup; }
    fputs("Manifest {\n  format: axi-wrap-manifest-v1\n  session_id: native-axi-test\n  issue_ids: []\n  context_ids: []\n  paths: [\"payload.bin\"]\n}\n", stream);
    if (fclose(stream) != 0) { failures = 1; goto cleanup; }
    snprintf(command, sizeof(command), "\"\"%s\" wrap --root \"%s\"\"", output, fixture);
    if (system(command) != 0) { fprintf(stderr, "FAIL dvcs wrap\n"); failures = 1; goto cleanup; }
    snprintf(path, sizeof(path), "%s\\.axi\\refs\\wip\\native-axi-test", fixture);
    stream = fopen(path, "rb");
    if (!stream) { failures = 1; goto cleanup; }
    count = fread(ref, 1, sizeof(ref), stream);
    fclose(stream);
    if (count != 65 || ref[64] != '\n') { fprintf(stderr, "FAIL dvcs ref format\n"); failures = 1; goto cleanup; }
    for (i = 0; i < 64; ++i) if (!((ref[i] >= '0' && ref[i] <= '9') || (ref[i] >= 'a' && ref[i] <= 'f'))) {
        fprintf(stderr, "FAIL dvcs ref digest\n"); failures = 1; goto cleanup;
    }
    snprintf(restore, sizeof(restore), "%s\\restore", fixture);
    if (!CreateDirectoryA(restore, NULL)) { failures = 1; goto cleanup; }
    snprintf(command, sizeof(command),
             "\"\"%s\" resume --root \"%s\" --session native-axi-test --to \"%s\"\"",
             output, fixture, restore);
    if (system(command) != 0) { fprintf(stderr, "FAIL dvcs resume\n"); failures = 1; goto cleanup; }
    snprintf(path, sizeof(path), "%s\\payload.bin", restore);
    stream = fopen(path, "rb");
    if (!stream) { failures = 1; goto cleanup; }
    memset(payload, 0, sizeof(payload));
    count = fread(payload, 1, sizeof(payload), stream);
    fclose(stream);
    if (count != 7 || memcmp(payload, "DVCS_OK", 7) != 0) {
        fprintf(stderr, "FAIL dvcs restored bytes\n"); failures = 1; goto cleanup;
    }
    printf("PASS dvcs init-wrap-resume durable round trip\n");

cleanup:
    if (strncmp(fixture, temp_base, strlen(temp_base)) != 0 || strstr(fixture, "..")) {
        fprintf(stderr, "FAIL refusing unsafe fixture cleanup\n");
        exit(1);
    }
    snprintf(command, sizeof(command), "rmdir /s /q \"%s\"", fixture);
    if (system(command) != 0) failures = 1;
    if (failures) exit(1);
    printf("PASS native .axi DVCS verification\n");
    """

start -> build_and_verify_dvcs
