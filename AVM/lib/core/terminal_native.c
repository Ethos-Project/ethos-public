#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

enum {
    AXI_TERMINAL_PROMPT_CAPACITY = 512,
    AXI_TERMINAL_HISTORY_CAPACITY = 32,
    AXI_TERMINAL_SCROLLBACK_CAPACITY = 128,
    AXI_TERMINAL_LINE_CAPACITY = 512
};

struct AxiTerminal {
    char prompt[AXI_TERMINAL_PROMPT_CAPACITY];
    char workspace[MAX_PATH];
    char history[AXI_TERMINAL_HISTORY_CAPACITY][AXI_TERMINAL_PROMPT_CAPACITY];
    size_t history_count;
    size_t history_cursor;
    char lines[AXI_TERMINAL_SCROLLBACK_CAPACITY][AXI_TERMINAL_LINE_CAPACITY];
    size_t line_count;
};

static void copy_text(char *destination, size_t capacity, const char *source) {
    size_t length;
    if (!destination || !capacity) return;
    if (!source) {
        destination[0] = '\0';
        return;
    }
    length = strlen(source);
    if (length >= capacity) length = capacity - 1;
    memcpy(destination, source, length);
    destination[length] = '\0';
}

static void append_line(AxiTerminal *terminal, const char *text) {
    size_t index;
    if (!terminal) return;
    if (terminal->line_count == AXI_TERMINAL_SCROLLBACK_CAPACITY) {
        memmove(terminal->lines, terminal->lines + 1,
                (AXI_TERMINAL_SCROLLBACK_CAPACITY - 1) * AXI_TERMINAL_LINE_CAPACITY);
        terminal->line_count--;
    }
    index = terminal->line_count++;
    copy_text(terminal->lines[index], AXI_TERMINAL_LINE_CAPACITY, text);
}

static void record_history(AxiTerminal *terminal, const char *command) {
    if (terminal->history_count == AXI_TERMINAL_HISTORY_CAPACITY) {
        memmove(terminal->history, terminal->history + 1,
                (AXI_TERMINAL_HISTORY_CAPACITY - 1) * AXI_TERMINAL_PROMPT_CAPACITY);
        terminal->history_count--;
    }
    copy_text(terminal->history[terminal->history_count], AXI_TERMINAL_PROMPT_CAPACITY, command);
    terminal->history_count++;
    terminal->history_cursor = terminal->history_count;
}

static const char *command_name(const char *prompt) {
    const char *cursor = prompt;
    while (*cursor == ' ' || *cursor == '\t') cursor++;
    if (strncmp(cursor, "axi", 3) != 0 ||
        (cursor[3] != '\0' && cursor[3] != ' ' && cursor[3] != '\t')) return NULL;
    cursor += 3;
    while (*cursor == ' ' || *cursor == '\t') cursor++;
    return cursor;
}

static void append_output(AxiTerminal *terminal, const char *output) {
    char line[AXI_TERMINAL_LINE_CAPACITY];
    size_t length = 0;
    while (*output) {
        if (*output == '\r') {
            output++;
            continue;
        }
        if (*output == '\n' || length + 1 == sizeof(line)) {
            line[length] = '\0';
            if (length) append_line(terminal, line);
            length = 0;
            if (*output == '\n') output++;
            continue;
        }
        line[length++] = *output++;
    }
    if (length) {
        line[length] = '\0';
        append_line(terminal, line);
    }
}

static int append_argument(char *command, size_t capacity, size_t *used, const char *argument) {
    size_t index;
    if (strchr(argument, '"')) return 0;
    if (*used + 3 >= capacity) return 0;
    command[(*used)++] = '"';
    for (index = 0; argument[index]; ++index) {
        if (*used + 2 >= capacity) return 0;
        command[(*used)++] = argument[index];
    }
    command[(*used)++] = '"';
    command[(*used)++] = ' ';
    command[*used] = '\0';
    return 1;
}

static int run_process(AxiTerminal *terminal, const char *program,
                       const char *const arguments[], size_t argument_count) {
    SECURITY_ATTRIBUTES security = { sizeof(security), NULL, TRUE };
    STARTUPINFOA startup;
    PROCESS_INFORMATION process;
    HANDLE read_pipe = NULL;
    HANDLE write_pipe = NULL;
    char command[2048] = { 0 };
    char output[1024];
    DWORD read = 0;
    DWORD exit_code = 0;
    size_t index;
    size_t used = 0;

    if (!append_argument(command, sizeof(command), &used, program)) return -1;
    for (index = 0; index < argument_count; ++index) {
        if (!append_argument(command, sizeof(command), &used, arguments[index])) return -1;
    }
    if (!CreatePipe(&read_pipe, &write_pipe, &security, 0) ||
        !SetHandleInformation(read_pipe, HANDLE_FLAG_INHERIT, 0)) return -1;

    ZeroMemory(&startup, sizeof(startup));
    ZeroMemory(&process, sizeof(process));
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput = write_pipe;
    startup.hStdError = write_pipe;
    if (!CreateProcessA(program, command, NULL, NULL, TRUE, CREATE_NO_WINDOW,
                        NULL, NULL, &startup, &process)) {
        CloseHandle(read_pipe);
        CloseHandle(write_pipe);
        return -1;
    }

    CloseHandle(write_pipe);
    while (ReadFile(read_pipe, output, sizeof(output) - 1, &read, NULL) && read) {
        output[read] = '\0';
        append_output(terminal, output);
    }
    WaitForSingleObject(process.hProcess, INFINITE);
    GetExitCodeProcess(process.hProcess, &exit_code);
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    CloseHandle(read_pipe);
    return (int)exit_code;
}

static int configured_compiler(char compiler[MAX_PATH]) {
    DWORD length = GetEnvironmentVariableA("AXI_COMPILER_PATH", compiler, MAX_PATH);
    return length > 0 && length < MAX_PATH && GetFileAttributesA(compiler) != INVALID_FILE_ATTRIBUTES;
}

static int configured_dvcs(char dvcs[MAX_PATH]) {
    DWORD length = GetEnvironmentVariableA("AXI_DVCS_PATH", dvcs, MAX_PATH);
    if (length > 0 && length < MAX_PATH &&
        GetFileAttributesA(dvcs) != INVALID_FILE_ATTRIBUTES) return 1;
    length = SearchPathA(NULL, "axi.exe", NULL, MAX_PATH, dvcs, NULL);
    return length > 0 && length < MAX_PATH &&
           GetFileAttributesA(dvcs) != INVALID_FILE_ATTRIBUTES;
}

static int resolve_workspace_root(const char *requested, char root[MAX_PATH]) {
    DWORD length;
    if (requested && *requested) {
        length = GetFullPathNameA(requested, MAX_PATH, root, NULL);
    } else {
        length = GetCurrentDirectoryA(MAX_PATH, root);
    }
    if (!length || length >= MAX_PATH) return 0;
    return (GetFileAttributesA(root) & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

static int execute_init(AxiTerminal *terminal, char *tokens[], size_t count) {
    char dvcs[MAX_PATH];
    char root[MAX_PATH];
    const char *arguments[3];
    int result;

    if (count != 2) {
        append_line(terminal, "axi: workspace path is required. Usage: axi init <workspace-path>");
        return 2;
    }
    if (!configured_dvcs(dvcs)) {
        append_line(terminal, "axi: DVCS is not configured. Set AXI_DVCS_PATH or add axi.exe to PATH.");
        return 2;
    }
    if (!resolve_workspace_root(tokens[1], root)) {
        append_line(terminal, "axi: workspace path is invalid or unavailable.");
        return 2;
    }

    arguments[0] = "init";
    arguments[1] = "--root";
    arguments[2] = root;
    result = run_process(terminal, dvcs, arguments, 3);
    if (result) {
        append_line(terminal, "axi: init failed.");
        return result < 0 ? 2 : result;
    }
    copy_text(terminal->workspace, sizeof(terminal->workspace), root);
    append_line(terminal, "axi: workspace initialized.");
    return 0;
}

static int execute_wrap(AxiTerminal *terminal, char *tokens[], size_t count) {
    char dvcs[MAX_PATH];
    char root[MAX_PATH];
    const char *arguments[3];
    int result;

    if (count > 2) {
        append_line(terminal, "axi: usage: axi wrap [workspace-path]");
        return 2;
    }
    if (!configured_dvcs(dvcs)) {
        append_line(terminal, "axi: DVCS is not configured. Set AXI_DVCS_PATH or add axi.exe to PATH.");
        return 2;
    }
    if (count == 2) {
        if (!resolve_workspace_root(tokens[1], root)) {
            append_line(terminal, "axi: workspace path is invalid or unavailable.");
            return 2;
        }
    } else if (terminal->workspace[0]) {
        copy_text(root, sizeof(root), terminal->workspace);
    } else {
        append_line(terminal, "axi: no workspace is configured. Run 'axi init <workspace-path>' first.");
        return 2;
    }

    arguments[0] = "wrap";
    arguments[1] = "--root";
    arguments[2] = root;
    result = run_process(terminal, dvcs, arguments, 3);
    if (result) {
        append_line(terminal, "axi: wrap failed.");
        return result < 0 ? 2 : result;
    }
    copy_text(terminal->workspace, sizeof(terminal->workspace), root);
    append_line(terminal, "axi: workspace wrapped.");
    return 0;
}

static int derive_output_path(const char *input, char output[MAX_PATH]) {
    const char *slash = strrchr(input, '\\');
    const char *alternate_slash = strrchr(input, '/');
    const char *extension = strrchr(input, '.');
    size_t length;
    if (alternate_slash && (!slash || alternate_slash > slash)) slash = alternate_slash;
    if (!extension || (slash && extension < slash)) return 0;
    length = (size_t)(extension - input);
    if (length + 5 > MAX_PATH) return 0;
    memcpy(output, input, length);
    memcpy(output + length, ".exe", 5);
    return 1;
}

static size_t split_tokens(char *text, char *tokens[], size_t capacity) {
    size_t count = 0;
    char *cursor = text;
    while (*cursor && count < capacity) {
        char quote = '\0';
        while (*cursor == ' ' || *cursor == '\t') cursor++;
        if (!*cursor) break;
        if (*cursor == '"') quote = *cursor++;
        tokens[count++] = cursor;
        while (*cursor && (quote ? *cursor != quote : (*cursor != ' ' && *cursor != '\t'))) cursor++;
        if (!*cursor) break;
        *cursor++ = '\0';
    }
    return count;
}

static int execute_build(AxiTerminal *terminal, const char *input, const char *output) {
    char compiler[MAX_PATH];
    const char *arguments[2];
    int result;
    if (!configured_compiler(compiler)) {
        append_line(terminal, "axi: compiler is not configured. Set AXI_COMPILER_PATH.");
        return 2;
    }
    arguments[0] = input;
    arguments[1] = output;
    result = run_process(terminal, compiler, arguments, 2);
    if (result) {
        append_line(terminal, "axi: build failed.");
        return result < 0 ? 2 : result;
    }
    append_line(terminal, "axi: build completed.");
    return 0;
}

static int execute_command(AxiTerminal *terminal, char *tokens[], size_t count, int run) {
    char output[MAX_PATH];
    int debug = 0;
    int result;
    size_t index;
    if (count < 2) {
        append_line(terminal, "axi: build and run require a source file.");
        return 2;
    }
    for (index = 2; index < count; ++index) {
        if (strcmp(tokens[index], "--debug") == 0 && run) debug = 1;
        else {
            append_line(terminal, "axi: unsupported build or run option.");
            return 2;
        }
    }
    if (!derive_output_path(tokens[1], output)) {
        append_line(terminal, "axi: unable to derive an output path.");
        return 2;
    }
    result = execute_build(terminal, tokens[1], output);
    if (result || !run) return result;
    result = run_process(terminal, output, NULL, 0);
    if (result) {
        append_line(terminal, "axi: run failed.");
        return result < 0 ? 2 : result;
    }
    append_line(terminal, debug ? "axi: run completed; artifacts retained." : "axi: run completed.");
    return 0;
}

AxiTerminal *axi_terminal_create(void) {
    AxiTerminal *terminal = (AxiTerminal *)calloc(1, sizeof(*terminal));
    return terminal;
}

void axi_terminal_destroy(AxiTerminal *terminal) {
    free(terminal);
}

void axi_terminal_insert_text(AxiTerminal *terminal, const char *text) {
    size_t existing;
    size_t available;
    size_t length;
    if (!terminal || !text) return;
    existing = strlen(terminal->prompt);
    available = AXI_TERMINAL_PROMPT_CAPACITY - existing - 1;
    length = strlen(text);
    if (length > available) length = available;
    memcpy(terminal->prompt + existing, text, length);
    terminal->prompt[existing + length] = '\0';
}

void axi_terminal_backspace(AxiTerminal *terminal) {
    size_t length;
    if (!terminal) return;
    length = strlen(terminal->prompt);
    if (length) terminal->prompt[length - 1] = '\0';
}

void axi_terminal_history_previous(AxiTerminal *terminal) {
    if (!terminal || !terminal->history_count) return;
    if (terminal->history_cursor) terminal->history_cursor--;
    copy_text(terminal->prompt, AXI_TERMINAL_PROMPT_CAPACITY,
              terminal->history[terminal->history_cursor]);
}

void axi_terminal_history_next(AxiTerminal *terminal) {
    if (!terminal || !terminal->history_count) return;
    if (terminal->history_cursor < terminal->history_count) terminal->history_cursor++;
    if (terminal->history_cursor == terminal->history_count) {
        terminal->prompt[0] = '\0';
        return;
    }
    copy_text(terminal->prompt, AXI_TERMINAL_PROMPT_CAPACITY,
              terminal->history[terminal->history_cursor]);
}

int axi_terminal_submit(AxiTerminal *terminal) {
    const char *command;
    char command_copy[AXI_TERMINAL_PROMPT_CAPACITY];
    char *tokens[8];
    size_t token_count;
    char compiler[MAX_PATH];
    char command_word[AXI_TERMINAL_LINE_CAPACITY];
    char unknown[AXI_TERMINAL_LINE_CAPACITY];
    size_t length = 0;
    if (!terminal) return 2;
    command = command_name(terminal->prompt);
    if (!command) {
        append_line(terminal, "axi: commands must begin with 'axi'. Try 'axi help'.");
        terminal->prompt[0] = '\0';
        return 2;
    }
    if (!*command) {
        append_line(terminal, "axi: missing command. Try 'axi help'.");
        terminal->prompt[0] = '\0';
        return 2;
    }

    record_history(terminal, terminal->prompt);
    if (strncmp(command, "help", 4) == 0 && (command[4] == '\0' || command[4] == ' ')) {
        append_line(terminal, "Axi Terminal");
        append_line(terminal, "Commands: help, status, init, wrap, build, run");
        append_line(terminal, "Init: axi init <workspace-path>");
        append_line(terminal, "Wrap: axi wrap [workspace-path]");
        append_line(terminal, "Planned: track, ship, resume, inject");
        terminal->prompt[0] = '\0';
        return 0;
    }
    if (strncmp(command, "status", 6) == 0 && (command[6] == '\0' || command[6] == ' ')) {
        append_line(terminal, "Axi Terminal status");
        if (terminal->workspace[0]) {
            snprintf(command_word, sizeof(command_word), "Workspace: %s", terminal->workspace);
            append_line(terminal, command_word);
        } else {
            append_line(terminal, "Workspace: not configured");
        }
        append_line(terminal, configured_compiler(compiler) ? "Compiler: configured" : "Compiler: not configured");
        terminal->prompt[0] = '\0';
        return 0;
    }
    copy_text(command_copy, sizeof(command_copy), command);
    token_count = split_tokens(command_copy, tokens, sizeof(tokens) / sizeof(tokens[0]));
    if (token_count && strcmp(tokens[0], "init") == 0) {
        int result = execute_init(terminal, tokens, token_count);
        terminal->prompt[0] = '\0';
        return result;
    }
    if (token_count && strcmp(tokens[0], "wrap") == 0) {
        int result = execute_wrap(terminal, tokens, token_count);
        terminal->prompt[0] = '\0';
        return result;
    }
    if (token_count && strcmp(tokens[0], "build") == 0) {
        int result = execute_command(terminal, tokens, token_count, 0);
        terminal->prompt[0] = '\0';
        return result;
    }
    if (token_count && strcmp(tokens[0], "run") == 0) {
        int result = execute_command(terminal, tokens, token_count, 1);
        terminal->prompt[0] = '\0';
        return result;
    }

    while (command[length] && command[length] != ' ' && command[length] != '\t' &&
           length + 1 < sizeof(command_word)) {
        command_word[length] = command[length];
        length++;
    }
    command_word[length] = '\0';
    snprintf(unknown, sizeof(unknown), "axi: unknown command '%.470s'. Try 'axi help'.", command_word);
    append_line(terminal, unknown);
    terminal->prompt[0] = '\0';
    return 2;
}

const char *axi_terminal_prompt(const AxiTerminal *terminal) {
    return terminal ? terminal->prompt : "";
}

size_t axi_terminal_line_count(const AxiTerminal *terminal) {
    return terminal ? terminal->line_count : 0;
}

const char *axi_terminal_line_at(const AxiTerminal *terminal, size_t index) {
    if (!terminal || index >= terminal->line_count) return "";
    return terminal->lines[index];
}
