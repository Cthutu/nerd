//------------------------------------------------------------------------------
// Shell API
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <core/core.h>

//------------------------------------------------------------------------------

#if OS_WINDOWS

int shell(cstr command)
{
    STARTUPINFOA        si = {0};
    PROCESS_INFORMATION pi = {0};

    si.cb                  = sizeof(si);

    if (!CreateProcessA(
            NULL, (char*)command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        return -1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exit_code;
    GetExitCodeProcess(pi.hProcess, &exit_code);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return (int)exit_code;
}

#elif OS_POSIX

int shell(cstr command)
{
    int result = system(command);
    if (result == -1) {
        return -1;
    } else {
        return WEXITSTATUS(result);
    }
}

#else
#    error "Shell not implemented for this OS."
#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
