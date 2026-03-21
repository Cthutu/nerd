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

    si.cb = sizeof(si);

    if (!CreateProcessA(
            NULL, (char*)command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        return -1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exit_code = 0;
    GetExitCodeProcess(pi.hProcess, &exit_code);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return (int)exit_code;
}

ShellResult shell_capture(cstr command, Arena* arena)
{
    UNUSED(command);
    UNUSED(arena);
    return (ShellResult){
        .exit_code   = shell(command),
        .stdout_text = s(""),
        .stderr_text = s("shell_capture not implemented on Windows"),
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

    int stdout_fd = mkstemp(stdout_template);
    int stderr_fd = mkstemp(stderr_template);
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
    int exit_code = shell((cstr)wrapped_command.data);

    FileMap stdout_map  = {0};
    FileMap stderr_map  = {0};
    string  stdout_file = filemap_load(stdout_template, &stdout_map);
    string  stderr_file = filemap_load(stderr_template, &stderr_map);

    string stdout_text = {0};
    if (stdout_file.data || stdout_file.count == 0) {
        stdout_text.data = (u8*)arena_alloc(arena, stdout_file.count);
        stdout_text.count = stdout_file.count;
        memcpy(stdout_text.data, stdout_file.data, stdout_file.count);
    }

    string stderr_text = {0};
    if (stderr_file.data || stderr_file.count == 0) {
        stderr_text.data = (u8*)arena_alloc(arena, stderr_file.count);
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
