#!/usr/bin/env python3
"""Compare generated C at -O0 and -O2 with the LLVM backend."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import os
import json
import re
import shlex
from pathlib import Path
import subprocess
import tempfile

from test import case_platforms, current_platform, language_cases, language_flags

ROOT = Path(__file__).resolve().parents[1]
# Process-lifetime, argument and diagnostic contracts beyond language discovery.
COMMANDS = (
    'arena-borrowing',
    'slice-assignment-negative',
    'slice-assignment-reversed',
    'slice-assignment-end',
    'slice-assignment-source',
    'slice-assignment-length',
    'slice-assignment-empty',

    '331-runtime-array-negative',
    '332-runtime-array-overflow',
    '328-optional-main-failure',
    '329-optional-main-args',
    '218-run-core-abort',
    '223-run-never-on-expression',
    '228-run-core-option-expect',
    '229-run-core-option-expect-abort',
    '254-run-core-done-reports-leaks',
    '258-run-dynamic-array-leak-location',
    'dynamic-array-copy-leak-location',
    'dynamic-array-copy-aggregate',
    '259-run-lazy-arena-leak-location',
    '296-run-merged-standard-library',
    '308-run-shutdown-watcher',
    '359-result-main-void-error',
    '360-result-main-i32-success',
    '361-result-main-propagation',
    '362-optional-main-i32-success',
    '363-optional-main-i32-failure',
    '364-result-main-string-error',
    '365-result-main-void-success',
    '366-result-main-call-status',
)


def sections(path):
    return [part.removeprefix("\n").removesuffix("\n")
            for part in path.read_text(encoding="utf-8").split("¬")]


def run(args, env, *, check=True, stdin=None):
    result = subprocess.run([str(a) for a in args], env=env, cwd=ROOT,
                            input=stdin, capture_output=True, timeout=60)
    if check and result.returncode:
        raise AssertionError(f"{' '.join(map(str, args))}\n{result.stderr.decode(errors='replace')}")
    return result


def behaviour(result):
    return result.returncode, result.stdout, result.stderr


def differential(nerd, fixture, tmp, env):
    cases = language_cases(fixture) if fixture.suffix == ".t" else [("program", sections(fixture))]
    work = tmp / fixture.stem
    work.mkdir()
    failures = []
    for label, parts in cases:
        case_work = work / label
        case_work.mkdir()
        try:
            differential_case(nerd, fixture, parts, case_work, env)
        except Exception as error:
            failures.append(f"{label}: {error}")
    assert not failures, "\n".join(failures)
    return f"[PASS] C: {fixture.name} ({len(cases)} programs, -O0, -O2)"


def differential_case(nerd, fixture, parts, work, env):
    source = work / (fixture.stem + ".n")
    # Resolve literal assets against the fixture before relocating its program.
    def embedded_asset(match):
        asset = fixture.parent / json.loads(match[1])
        return "@embed(" + json.dumps(str(asset.resolve())) + ")"
    source.write_text(re.sub(r'@embed\(("[^"\n]+")\)', embedded_asset, parts[0]), encoding="utf-8")
    stdin = parts[5].encode() if fixture.suffix == ".t" and len(parts) > 5 else None
    output = work / ("program.exe" if os.name == "nt" else "program")
    run([nerd, "build", *language_flags(parts[0]), "-o", output, source], env)
    expected = run([output], env, check=False, stdin=stdin)
    output.unlink()
    generated_options = run([nerd, "build", *language_flags(parts[0]),
                             "--cgen", "--copts", "-o", output, source], env)
    copts = shlex.split(generated_options.stdout.decode("utf-8"))
    assert not output.exists(), "C generation must not create a binary"
    generated = output.with_suffix(".c")
    assert generated.is_file(), "C output must replace the binary extension"
    for optimisation in ("-O0", "-O2"):
        run(["clang", "-std=gnu11", optimisation,
             "-Werror", generated, "-o", output, *copts], env)
        actual = run([output], env, check=False, stdin=stdin)
        assert behaviour(actual) == behaviour(expected), (
            f"{fixture.name} {optimisation}: C and LLVM behaviour differs\n"
            f"C: {behaviour(actual)!r}\nLLVM: {behaviour(expected)!r}")


def cli_checks(nerd, tmp, env):
    work = tmp / "paths with spaces"
    work.mkdir()
    source = work / "hello.n"
    source.write_text('main :: fn () { prn("generated C") }\n', encoding="utf-8")
    legacy = run([nerd, "build", "--genc", source], env, check=False)
    assert legacy.returncode and not source.with_suffix(".c").exists()
    # Generation is independent of clang and the runtime object cache.
    no_compiler = {**env, "PATH": str(work / "empty-path")}
    run([nerd, "build", "--cgen", "--hir", source], no_compiler)
    assert source.with_suffix(".c").is_file()
    assert list(work.glob("*.hir")), "--cgen --hir must preserve the HIR sidecar"
    assert not source.with_suffix(".exe" if os.name == "nt" else "").exists()
    for suffix in ("", ".exe", ".out", ".c"):
        output = work / ("custom" + suffix)
        run([nerd, "build", "--cgen", "-o", output, source], env)
        assert output.with_suffix(".c").is_file()
    snippet_output = work / "snippet"
    run([nerd, "build", "--cgen", "-o", snippet_output,
         'main :: fn (args: []string) -> i32 { assert args.count == 3\n assert args[1] == "one"\n assert args[2] == "two words"\n return 7 }'], env)
    executable = work / ("snippet.exe" if os.name == "nt" else "snippet")
    run(["clang", snippet_output.with_suffix(".c"), "-o", executable], env)
    result = run([executable, "one", "two words"], env, check=False)
    assert behaviour(result) == (7, b"", b""), "generated main must forward arguments and exit status"
    invalid = work / "invalid.n"
    invalid.write_text("main :: fn () { missing_symbol() }", encoding="utf-8")
    result = run([nerd, "build", "--cgen", invalid], env, check=False)
    assert result.returncode and not invalid.with_suffix(".c").exists()

    if os.name == "nt":
        import ctypes
        from ctypes import wintypes
        import threading
        kernel = ctypes.WinDLL('kernel32', use_last_error=True)
        kernel.CreateFileW.argtypes = [wintypes.LPCWSTR, wintypes.DWORD, wintypes.DWORD,
                                      wintypes.LPVOID, wintypes.DWORD, wintypes.DWORD, wintypes.HANDLE]
        kernel.CreateFileW.restype = wintypes.HANDLE
        kernel.CloseHandle.argtypes = [wintypes.HANDLE]
        generated = source.with_suffix('.c')
        command = [nerd, 'build', '--cgen', '-o', generated, source]
        run(command, env)
        reference = generated.read_bytes()

        def lock_output():
            handle = kernel.CreateFileW(str(generated), 0x80000000, 1, None, 3, 0, None)
            assert handle != ctypes.c_void_p(-1).value, ctypes.get_last_error()
            return handle

        # A stable sharing violation must fail without truncating old output.
        handle = lock_output()
        try:
            failed = run(command, env, check=False)
            assert failed.returncode and b'Failed to open C output' in failed.stderr
            assert generated.read_bytes() == reference
        finally:
            kernel.CloseHandle(handle)
        # A temporary reader releases its lock while the compiler is opening
        # the file; the eventual write must remain byte-identical.
        handle = lock_output()
        released = threading.Timer(0.2, kernel.CloseHandle, args=[handle])
        released.start()
        try:
            run(command, env)
        finally:
            released.join()
        assert generated.read_bytes() == reference
        print('[PASS] Windows C output: temporary sharing retry and persistent-lock failure', flush=True)


def terminal_frame(executable, env):
    # Read a complete frame before sending any input. A test that sends Q first
    # can mistake the shutdown flush for working presentation.
    import fcntl
    import pty
    import select
    import struct
    import termios
    import time

    master, slave = pty.openpty()
    proc = None
    data = bytearray()
    first_frame = None
    try:
        fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack("HHHH", 24, 80, 0, 0))
        proc = subprocess.Popen([str(executable)], stdin=slave, stdout=slave,
                                stderr=slave, env=env, cwd=ROOT, start_new_session=True)
        os.close(slave)
        slave = -1
        deadline = time.monotonic() + 15
        while time.monotonic() < deadline:
            if select.select([master], [], [], 0.05)[0]:
                try:
                    chunk = os.read(master, 65536)
                except OSError:
                    break
                if not chunk:
                    break
                data.extend(chunk)
                end = data.find(b"\x1b[0m")
                if first_frame is None and end >= 0:
                    first_frame = bytes(data[:end + 4])
                    os.write(master, b"q")
            if proc.poll() is not None:
                break
        assert first_frame is not None, (
            f"{executable}: no frame appeared before input; received {bytes(data[:160])!r}")
        assert b"rooms " in first_frame and b"seed 12345" in first_frame
        assert b"#" in first_frame and b"." in first_frame
        assert proc.wait(timeout=3) == 0, "Dungeon must exit successfully on Q"
        return first_frame
    finally:
        if proc is not None and proc.poll() is None:
            proc.kill()
            proc.wait()
        os.close(master)
        if slave >= 0:
            os.close(slave)


def dungeon_check(nerd, tmp, env):
    import sys
    if not sys.platform.startswith("linux"):
        print("[SKIP] Dungeon terminal regression requires Linux PTYs", flush=True)
        return
    work = tmp / "dungeon"
    work.mkdir()
    source = work / "dungeon.n"
    # Keep the actual example and library rendering path, but use a fixed seed
    # so the first frame can be compared byte for byte across backends.
    source.write_text((ROOT / "examples/dungeon/dungeon.n").read_text(encoding="utf-8").replace(
        "seed = now()", "seed = 12345"), encoding="utf-8")
    executable = work / "dungeon"
    run([nerd, "build", "-o", executable, source], env)
    expected = terminal_frame(executable, env)
    executable.unlink()
    run([nerd, "build", "--cgen", "-o", executable, source], env)
    for optimisation in ("-O0", "-O2"):
        run(["clang", "-Werror", optimisation, source.with_suffix(".c"), "-o", executable], env)
        actual = terminal_frame(executable, env)
        assert actual == expected, f"Dungeon {optimisation}: first frame differs between C and LLVM"
    print("[PASS] Dungeon renders before input, matches LLVM and quits on Q (-O0, -O2)", flush=True)


def copts_checks(nerd, tmp, env):
    import sys
    work = tmp / "copts with spaces"
    work.mkdir()
    source = work / "app.n"
    source.write_text('use std.math\nmain :: fn () { assert sin(0.0) == 0.0 }\n', encoding="utf-8")
    no_compiler = {**env, "PATH": str(work / "empty-path")}

    def options(*args, environment=env):
        result = run([nerd, "build", "--copts", *args], environment)
        assert not result.stderr, result.stderr
        assert len(result.stdout.splitlines()) == 1, result.stdout
        flags = result.stdout.decode().split()
        assert flags and all(flag.startswith("-") for flag in flags), flags
        return flags

    # Querying must not compile or remove any pre-existing outputs/sidecars.
    sentinels = [source.with_suffix(suffix) for suffix in (".c", ".exe", "")]
    sentinels += [work / "_app.hir", work / "_app.ll", work / "app.m1.ll"]
    for path in sentinels:
        path.write_text("preserve me", encoding="utf-8")
    debug = options("--hir", "--timing", "-v", source, environment=no_compiler)
    assert "-g" in debug and "-O0" in debug and "-std=gnu11" in debug
    release = options("-r", source)
    assert "-O2" in release and "-DNDEBUG" in release and "-g" not in release
    if sys.platform.startswith("linux"):
        assert debug.count("-lm") == 1
    else:
        assert "-lm" not in debug
    for path in sentinels:
        assert path.read_text(encoding="utf-8") == "preserve me", f"--copts modified {path}"
        path.unlink()
    flags = options("--cgen", "-r", source)
    executable = work / ("app.exe" if os.name == "nt" else "app")
    run(["clang", "-Werror", source.with_suffix(".c"), *flags, "-o", executable], env)
    run([executable], env)
    if os.name != "nt":
        run(["bash", "-c", 'clang "$1" $("$2" build --cgen --copts -r "$3") -o "$4"',
             "copts-test", source.with_suffix(".c"), nerd, source, executable], env)
        run([executable], env)
    invalid = run([nerd, "build", "--copts", "main :: fn () { missing() }"], env, check=False)
    assert invalid.returncode and not invalid.stdout
    windowed = options('build { windowed: yes }\nmain :: fn () {}')
    assert ("-Wl,/SUBSYSTEM:WINDOWS" in windowed) == (os.name == "nt")

    library = work / "library.n"
    library.write_text('''pub use test.library_exports
base: i32 = 10
pub add :: fn (a: i32, b: i32) -> i32 { return base + a + b }
foreign_abs :: ffi "c" abs (value: i32) -> i32
pub absolute :: foreign_abs
pub first :: fn (fixed: i32, args: ...) -> i32 { return fixed + args.next[i32]() }
''', encoding="utf-8")
    host = work / "host.c"
    host.write_text('''#include <assert.h>
extern int add(int, int), absolute(int), library_add(int, int), first(int, ...);
int main(void) {
    assert(add(20, 12) == 42);
    assert(absolute(-42) == 42);
    assert(library_add(20, 22) == 42);
    assert(first(20, 22) == 42);
    return 0;
}
''', encoding="utf-8")
    for mode in ("--obj", "--lib", "--dll"):
        flags = options("--cgen", mode, "-r", library)
        object_path = work / ("library.obj" if os.name == "nt" else "library.o")
        if mode == "--dll":
            assert "-shared" in flags or "-dynamiclib" in flags
            assert ("-fPIC" in flags) == (os.name != "nt")
            suffix = ".dll" if os.name == "nt" else ".dylib" if sys.platform == "darwin" else ".so"
            artifact = work / ("library" + suffix)
        else:
            assert "-c" in flags and not any(f.startswith("-l") for f in flags)
            artifact = object_path
        run(["clang", "-Werror", library.with_suffix(".c"), *flags, "-o", artifact], env)
        if mode == "--lib":
            artifact = work / ("library.lib" if os.name == "nt" else "library.a")
            if os.name == "nt":
                run(["llvm-lib", "/NOLOGO", f"/OUT:{artifact}", object_path], env)
            else:
                run(["ar", "rcs", artifact, object_path], env)
        # On Windows clang emits an import library alongside the DLL.
        link_artifact = artifact.with_suffix(".lib") if mode == "--dll" and os.name == "nt" else artifact
        run(["clang", host, link_artifact, "-o", executable], env)
        run([executable], env)
    if sys.platform.startswith("linux"):
        library.write_text(sections(ROOT / "tests/commands/297-build-variadic-host.cmd")[0], encoding="utf-8")
        flags = options("--cgen", "--dll", library)
        artifact = work / "variadic.so"
        run(["clang", "-Werror", library.with_suffix(".c"), *flags, "-o", artifact], env)
        run(["clang", ROOT / "tests/ffi/variadic_host.c", artifact, "-o", executable], env)
        assert run([executable], env).stdout == b"42\n"
    for mode in ("--obj", "--lib", "--dll"):
        rejected = run([nerd, "build", "--cgen", "--copts", mode, "-o", work / "rejected",
                        'pub invalid :: fn (value: string) -> string { return value }'], env, check=False)
        assert rejected.returncode and not rejected.stdout
        assert not (work / "rejected.c").exists()
    print("[PASS] --copts queries, external libraries, release, object, archive and shared-library host calls", flush=True)


def graphics_compile_checks(nerd, tmp, env):
    # Exercise default source discovery and real graphics imports without needing
    # a display server in CI. Keep all generated artifacts in the test directory.
    for name in ("pixels", "pixels_fit", "pixels_layers"):
        work = tmp / name
        work.mkdir()
        (work / (name + ".n")).write_bytes((ROOT / "examples" / name / (name + ".n")).read_bytes())
        generated = subprocess.run([str(nerd), "b", "--cgen", "--copts"],
                                   cwd=work, env=env, capture_output=True, timeout=60)
        assert generated.returncode == 0, generated.stderr.decode(errors="replace")
        flags = generated.stdout.decode().split()
        output = work / (name + (".exe" if os.name == "nt" else ""))
        run(["clang", work / (name + ".c"), *flags, "-Werror", "-o", output], env)
    print("[PASS] Pixels examples: default source discovery and standalone C compilation", flush=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--nerd", type=Path, default=ROOT / "_bin/nerd-debug")
    parser.add_argument("--all", action="store_true", help="Retained for compatibility; all language fixtures run by default")
    parser.add_argument("--jobs", type=int, default=4)
    options = parser.parse_args()
    nerd = options.nerd.resolve()
    env = {**os.environ, "NERD_LIB_PATH": os.pathsep.join((str(ROOT / "tests/mods"), str(ROOT / "mods")))}
    fixtures = sorted((ROOT / "tests/language").glob("*.t"))
    fixtures += [ROOT / "tests/commands" / (name + ".cmd") for name in COMMANDS]
    fixtures += sorted((ROOT / "tests/cgen").glob("*.n"))
    skipped = [fixture for fixture in fixtures
               if case_platforms(fixture) and current_platform() not in case_platforms(fixture)]
    for fixture in skipped:
        print(f"[SKIP] C: {fixture.name} (declared platforms: {', '.join(sorted(case_platforms(fixture)))})", flush=True)
    fixtures = [fixture for fixture in fixtures if fixture not in skipped]
    (ROOT / "_tmp").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cgen-", dir=ROOT / "_tmp") as directory:
        tmp = Path(directory)
        failures = []
        with ThreadPoolExecutor(max_workers=options.jobs) as pool:
            futures = [(f, pool.submit(differential, nerd, f, tmp, env)) for f in fixtures]
            for fixture, future in futures:
                try:
                    print(future.result(), flush=True)
                except Exception as error:
                    failures.append(f"{fixture.name}: {error}")
                    print(f"[FAIL] {failures[-1]}", flush=True)
        assert not failures, "\n".join(failures)
        cli_checks(nerd, tmp, env)
        copts_checks(nerd, tmp, env)
        graphics_compile_checks(nerd, tmp, env)
        dungeon_check(nerd, tmp, env)
        for flags in (("--llvm",), ("--obj", "--lib"), ("--lib", "--dll"), ("--obj", "--dll")):
            for cflag in ("--cgen", "--copts"):
                result = run([nerd, "build", cflag, *flags, "main :: fn () {}"], env, check=False)
                assert result.returncode and not result.stdout, f"{cflag} must reject {flags}"
    programs = sum(len(language_cases(fixture)) if fixture.suffix == ".t" else 1
                   for fixture in fixtures)
    print(f"C generation: {len(fixtures)} files / {programs} programs at two optimisation levels and CLI conflicts passed; {len(skipped)} platform file skips")


if __name__ == "__main__":
    main()
