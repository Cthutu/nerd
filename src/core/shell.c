//------------------------------------------------------------------------------
// Shell API
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <core/core.h>

#if OS_POSIX
#    include <stdio.h>
#endif

//------------------------------------------------------------------------------

#if OS_WINDOWS

int shell(cstr command)
{
    STARTUPINFOA        si = {0};
    PROCESS_INFORMATION pi = {0};

    si.cb                  = sizeof(si);
    char* command_line     = (char*)ALLOC(strlen(command) + 1);
    strcpy(command_line, command);

    if (!CreateProcessA(
            NULL, command_line, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        FREE(command_line);
        return -1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exit_code = 0;
    GetExitCodeProcess(pi.hProcess, &exit_code);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    FREE(command_line);

    return (int)exit_code;
}

ShellResult shell_capture(cstr command, Arena* arena)
{
    char temp_dir[MAX_PATH] = {0};
    if (GetTempPathA(sizeof(temp_dir), temp_dir) == 0) {
        return (ShellResult){
            .exit_code   = -1,
            .stdout_text = s(""),
            .stderr_text = s("Failed to get temporary directory"),
        };
    }

    char stdout_path[MAX_PATH] = {0};
    char stderr_path[MAX_PATH] = {0};
    if (GetTempFileNameA(temp_dir, "nrd", 0, stdout_path) == 0 ||
        GetTempFileNameA(temp_dir, "nrd", 0, stderr_path) == 0) {
        return (ShellResult){
            .exit_code   = -1,
            .stdout_text = s(""),
            .stderr_text = s("Failed to create temporary files"),
        };
    }

    SECURITY_ATTRIBUTES sa = {
        .nLength              = sizeof(sa),
        .lpSecurityDescriptor = NULL,
        .bInheritHandle       = TRUE,
    };
    HANDLE stdout_handle = CreateFileA(stdout_path,
                                       GENERIC_WRITE,
                                       FILE_SHARE_READ | FILE_SHARE_WRITE,
                                       &sa,
                                       CREATE_ALWAYS,
                                       FILE_ATTRIBUTE_TEMPORARY,
                                       NULL);
    HANDLE stderr_handle = CreateFileA(stderr_path,
                                       GENERIC_WRITE,
                                       FILE_SHARE_READ | FILE_SHARE_WRITE,
                                       &sa,
                                       CREATE_ALWAYS,
                                       FILE_ATTRIBUTE_TEMPORARY,
                                       NULL);
    if (stdout_handle == INVALID_HANDLE_VALUE ||
        stderr_handle == INVALID_HANDLE_VALUE) {
        if (stdout_handle != INVALID_HANDLE_VALUE) {
            CloseHandle(stdout_handle);
        }
        if (stderr_handle != INVALID_HANDLE_VALUE) {
            CloseHandle(stderr_handle);
        }
        path_remove(stdout_path);
        path_remove(stderr_path);
        return (ShellResult){
            .exit_code   = -1,
            .stdout_text = s(""),
            .stderr_text = s("Failed to open temporary files"),
        };
    }

    char* command_line = (char*)ALLOC(strlen(command) + 1);
    strcpy(command_line, command);

    HANDLE stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
    char*  redirect     = strstr(command_line, " < ");
    if (redirect != NULL) {
        *redirect        = '\0';
        char* input_path = redirect + 3;
        while (*input_path == ' ' || *input_path == '\t') {
            input_path++;
        }
        if (*input_path == '"') {
            input_path++;
            char* end_quote = strchr(input_path, '"');
            if (end_quote != NULL) {
                *end_quote = '\0';
            }
        } else {
            char* end = input_path;
            while (*end != '\0' && *end != ' ' && *end != '\t') {
                end++;
            }
            *end = '\0';
        }
        stdin_handle = CreateFileA(input_path,
                                   GENERIC_READ,
                                   FILE_SHARE_READ,
                                   &sa,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);
    }

    STARTUPINFOA        si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb                  = sizeof(si);
    si.dwFlags             = STARTF_USESTDHANDLES;
    si.hStdInput           = stdin_handle;
    si.hStdOutput          = stdout_handle;
    si.hStdError           = stderr_handle;

    int exit_code          = -1;
    if (stdin_handle != INVALID_HANDLE_VALUE &&
        CreateProcessA(
            NULL, command_line, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD process_exit_code = 0;
        GetExitCodeProcess(pi.hProcess, &process_exit_code);
        exit_code = (int)process_exit_code;

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    if (redirect != NULL && stdin_handle != INVALID_HANDLE_VALUE) {
        CloseHandle(stdin_handle);
    }
    CloseHandle(stdout_handle);
    CloseHandle(stderr_handle);
    FREE(command_line);

    FileMap stdout_map  = {0};
    FileMap stderr_map  = {0};
    string  stdout_file = filemap_load(stdout_path, &stdout_map);
    string  stderr_file = filemap_load(stderr_path, &stderr_map);

    string stdout_text  = {0};
    if (stdout_file.data || stdout_file.count == 0) {
        stdout_text.data = (u8*)arena_alloc(arena, stdout_file.count);
        for (usize i = 0; i < stdout_file.count; ++i) {
            if (stdout_file.data[i] != '\r') {
                stdout_text.data[stdout_text.count++] = stdout_file.data[i];
            }
        }
    }

    string stderr_text = {0};
    if (stderr_file.data || stderr_file.count == 0) {
        stderr_text.data = (u8*)arena_alloc(arena, stderr_file.count);
        for (usize i = 0; i < stderr_file.count; ++i) {
            if (stderr_file.data[i] != '\r') {
                stderr_text.data[stderr_text.count++] = stderr_file.data[i];
            }
        }
    }

    filemap_unload(&stdout_map);
    filemap_unload(&stderr_map);
    path_remove(stdout_path);
    path_remove(stderr_path);

    return (ShellResult){
        .exit_code   = exit_code,
        .stdout_text = stdout_text,
        .stderr_text = stderr_text,
    };
}

#elif OS_POSIX

int shell(cstr command)
{
    int result = system(command);
    if (result == -1) {
        return -1;
    }
    return WEXITSTATUS(result);
}

ShellResult shell_capture(cstr command, Arena* arena)
{
    char stdout_template[] = "/tmp/nerd-shell-stdout-XXXXXX";
    char stderr_template[] = "/tmp/nerd-shell-stderr-XXXXXX";

    int stdout_fd          = mkstemp(stdout_template);
    int stderr_fd          = mkstemp(stderr_template);
    if (stdout_fd < 0 || stderr_fd < 0) {
        kill("Failed to create temporary files for shell_capture");
    }

    close(stdout_fd);
    close(stderr_fd);

    string wrapped_command = string_format(&temp_arena,
                                           "( %s ) >\"%s\" 2>\"%s\"",
                                           command,
                                           stdout_template,
                                           stderr_template);
    int    exit_code       = shell((cstr)wrapped_command.data);

    FileMap stdout_map     = {0};
    FileMap stderr_map     = {0};
    string  stdout_file    = filemap_load(stdout_template, &stdout_map);
    string  stderr_file    = filemap_load(stderr_template, &stderr_map);

    string stdout_text     = {0};
    if (stdout_file.data || stdout_file.count == 0) {
        stdout_text.data  = (u8*)arena_alloc(arena, stdout_file.count);
        stdout_text.count = stdout_file.count;
        memcpy(stdout_text.data, stdout_file.data, stdout_file.count);
    }

    string stderr_text = {0};
    if (stderr_file.data || stderr_file.count == 0) {
        stderr_text.data  = (u8*)arena_alloc(arena, stderr_file.count);
        stderr_text.count = stderr_file.count;
        memcpy(stderr_text.data, stderr_file.data, stderr_file.count);
    }

    filemap_unload(&stdout_map);
    filemap_unload(&stderr_map);
    remove(stdout_template);
    remove(stderr_template);

    return (ShellResult){
        .exit_code   = exit_code,
        .stdout_text = stdout_text,
        .stderr_text = stderr_text,
    };
}

#else
#    error "Shell not implemented for this OS."
#endif

//------------------------------------------------------------------------------
