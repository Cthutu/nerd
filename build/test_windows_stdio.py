#!/usr/bin/env python3
"""Exercise GUI startup with a hidden parent console and inherited redirections."""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]

# A real parent console exercises AttachConsole, unlike capture_output alone.
# Python launches this helper with SW_HIDE; no visible test console is created.
PARENT = r'''
#include <windows.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc != 4) return 10;
    const char* mode = argv[1];
    FILE* report = NULL;
    if (fopen_s(&report, argv[3], "w") != 0) return 11;
    BOOL detached = strcmp(mode, "detached") == 0;
    if ((GetConsoleCP() == 0) != detached) return 12;
    STARTUPINFOA startup = {0};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION process = {0};
    SECURITY_ATTRIBUTES security = {sizeof(security), NULL, TRUE};
    HANDLE redirected = INVALID_HANDLE_VALUE;
    BOOL mixed_out = strcmp(mode, "mixed-out") == 0;
    BOOL mixed_err = strcmp(mode, "mixed-err") == 0;
    BOOL inherited = strcmp(mode, "inherited") == 0;
    HANDLE inherited_input = INVALID_HANDLE_VALUE;
    HANDLE inherited_output = INVALID_HANDLE_VALUE;
    char filename[MAX_PATH * 2];
    snprintf(filename, sizeof(filename), "%s.redirected", argv[3]);
    if (mixed_out || mixed_err) {
        redirected = CreateFileA(filename, GENERIC_WRITE, FILE_SHARE_READ,
                                 &security, CREATE_ALWAYS, 0, NULL);
        if (redirected == INVALID_HANDLE_VALUE) return 13;
        startup.dwFlags = STARTF_USESTDHANDLES;
        startup.hStdOutput = mixed_out ? redirected : NULL;
        startup.hStdError = mixed_err ? redirected : NULL;
    }
    if (inherited) {
        inherited_input = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE,
                                     FILE_SHARE_READ | FILE_SHARE_WRITE, &security,
                                     OPEN_EXISTING, 0, NULL);
        inherited_output = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE,
                                      FILE_SHARE_READ | FILE_SHARE_WRITE, &security,
                                      OPEN_EXISTING, 0, NULL);
        startup.dwFlags = STARTF_USESTDHANDLES;
        startup.hStdInput = inherited_input;
        startup.hStdOutput = inherited_output;
        startup.hStdError = inherited_output;
    }
    char command[MAX_PATH * 2];
    snprintf(command, sizeof(command), "\"%s\"", argv[2]);
    if (!CreateProcessA(NULL, command, NULL, NULL, mixed_out || mixed_err || inherited,
                        0, NULL, NULL, &startup, &process)) {
        fprintf(report, "CreateProcess: %lu\n", GetLastError());
        fclose(report);
        return 14;
    }
    HANDLE console = detached ? INVALID_HANDLE_VALUE :
        CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    char screen[32768] = {0};
    BOOL live = FALSE;
    DWORD status;
    for (int attempt = 0; attempt < 100; ++attempt) {
        status = WaitForSingleObject(process.hProcess, 20);
        if (!detached) {
            DWORD read = 0;
            ReadConsoleOutputCharacterA(console, screen, sizeof(screen)-1,
                                        (COORD){0,0}, &read);
            screen[read] = 0;
            if ((mixed_out || strstr(screen, "nerd-stdout-marker")) &&
                (mixed_err || strstr(screen, "nerd-stderr-marker")) &&
                status == WAIT_TIMEOUT) live = TRUE;
        }
        if (status == WAIT_OBJECT_0) break;
    }
    if (WaitForSingleObject(process.hProcess, 10000) != WAIT_OBJECT_0) {
        TerminateProcess(process.hProcess, 99);
        return 15;
    }
    DWORD exit_code;
    GetExitCodeProcess(process.hProcess, &exit_code);
    fprintf(report, "mode=%s live=%d exit=%lu\n", mode, live, exit_code);
    fprintf(report, "console stdout=%d stderr=%d\n",
            strstr(screen, "nerd-stdout-marker") != NULL,
            strstr(screen, "nerd-stderr-marker") != NULL);
    fclose(report);
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    if (console != INVALID_HANDLE_VALUE) CloseHandle(console);
    if (redirected != INVALID_HANDLE_VALUE) CloseHandle(redirected);
    if (inherited_input != INVALID_HANDLE_VALUE) CloseHandle(inherited_input);
    if (inherited_output != INVALID_HANDLE_VALUE) CloseHandle(inherited_output);
    if (exit_code != 0 || (!detached && !live)) return 16;
    if ((mixed_out && strstr(screen, "nerd-stdout-marker")) ||
        (mixed_err && strstr(screen, "nerd-stderr-marker"))) return 17;
    return 0;
}
'''

SOURCE = '''build { windowed: yes }
ffi "kernel32" {
    Sleep (milliseconds: u32)
    GetConsoleCP () -> u32
}
ffi "" { getchar () -> i32 }
main :: fn () -> i32 {
    on "detached" { on GetConsoleCP() != 0 => return 71 }
    on "input" { on getchar() != 120 => return 72 }
    prn("nerd-stdout-marker")
    eprn("nerd-stderr-marker")
    Sleep(1000)
    return 0
}
'''


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--nerd', type=Path, default=ROOT / '_bin/nerd-debug.exe')
    args = parser.parse_args()
    if os.name != 'nt':
        print('[SKIP] Windows GUI stdio requires native Windows')
        return
    nerd = args.nerd.resolve()
    env = dict(os.environ, NERD_LIB_PATH=str(ROOT / 'mods'))
    with tempfile.TemporaryDirectory(prefix='nerd windows stdio ') as directory:
        work = Path(directory)
        parent_c = work / 'parent.c'
        parent_c.write_text(PARENT, encoding='utf-8')
        parent = work / 'parent.exe'
        subprocess.run(['clang', str(parent_c), '-o', str(parent)], check=True)
        source = work / 'main.n'
        source.write_text(SOURCE, encoding='utf-8')
        hidden = subprocess.STARTUPINFO()
        hidden.dwFlags = subprocess.STARTF_USESHOWWINDOW
        hidden.wShowWindow = subprocess.SW_HIDE

        def compile_program(output, release, define=None, cgen=False):
            command = [str(nerd)] + ([f'-D{define}'] if define else [])
            command += ['build'] + (['-r'] if release else [])
            if cgen:
                cfile = output.with_suffix('.c')
                subprocess.run(command + ['--cgen', str(source), '-o', str(cfile)],
                               env=env, check=True, capture_output=True)
                subprocess.run(['clang', str(cfile), '-o', str(output),
                                '-O2' if release else '-O0',
                                '-Wl,/subsystem:windows'], check=True, capture_output=True)
            else:
                subprocess.run(command + [str(source), '-o', str(output)],
                               env=env, check=True, capture_output=True)

        for cgen in (False, True):
            for release in (False, True):
                label = f'{"cgen" if cgen else "llvm"}-{"release" if release else "debug"}'
                binary = work / (label + '.exe')
                compile_program(binary, release, cgen=cgen)
                for mode in ('console', 'inherited', 'mixed-out', 'mixed-err'):
                    report = work / (label + '-' + mode + '.log')
                    result = subprocess.run([str(parent), mode, str(binary), str(report)],
                                            creationflags=subprocess.CREATE_NEW_CONSOLE,
                                            startupinfo=hidden, env=env)
                    text = report.read_text() if report.exists() else 'No report'
                    assert result.returncode == 0, (label, mode, result.returncode, text)
                    if mode.startswith('mixed-'):
                        text = Path(str(report) + '.redirected').read_text()
                        expected = 'stdout' if mode == 'mixed-out' else 'stderr'
                        assert text == f'nerd-{expected}-marker\n', (label, mode, text)
                # No parent console: Explorer-style startup must not allocate one.
                compile_program(binary, release, define='detached', cgen=cgen)
                report = work / (label + '-detached.log')
                result = subprocess.run([str(parent), 'detached', str(binary), str(report)],
                                        creationflags=subprocess.DETACHED_PROCESS,
                                        startupinfo=hidden, env=env)
                assert result.returncode == 0, (label, report.read_text())
                # All three standard streams redirected, including stdin.
                compile_program(binary, release, define='input', cgen=cgen)
                result = subprocess.run([str(binary)], input='x', capture_output=True,
                                        text=True, env=env)
                assert (result.returncode, result.stdout, result.stderr) == (
                    0, 'nerd-stdout-marker\n', 'nerd-stderr-marker\n'), (label, result)
                with (work / 'out.txt').open('w') as out, (work / 'err.txt').open('w') as err:
                    result = subprocess.run([str(binary)], input='x', text=True,
                                            stdout=out, stderr=err, env=env)
                assert result.returncode == 0, label
                assert (work / 'out.txt').read_text() == 'nerd-stdout-marker\n'
                assert (work / 'err.txt').read_text() == 'nerd-stderr-marker\n'
                print(f'[PASS] {label}: parent console live output, mixed handles, detached, pipes/stdin, files', flush=True)


if __name__ == '__main__':
    main()
