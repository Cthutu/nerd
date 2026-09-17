#!/usr/bin/env python3
"""Compare generated C at -O0 and -O2 with the LLVM backend."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
# Runtime regressions beyond the language suite: defaults, ABI, ownership,
# atomics, allocator diagnostics, formatting and generic dispatch.
COMMANDS = (
    "309-run-on-implicit-binder-mutation",
    "210-run-fixed-array-implicit-slice",
    "211-run-implicit-integer-comparison-width",
    "212-run-large-array-field-index",
    "213-run-llvm-function-field-call",
    "214-run-counted-box-allocation",
    "215-run-box-field-return-ownership",
    "217-run-enum-braced-payload-aliased-binders",
    "217-run-string-bytes",
    "218-run-core-abort",
    "220-run-byte-arrays-to-string",
    "221-run-on-expression-return-option-variant",
    "222-run-on-expression-assignment-binder-read",
    "223-run-never-on-expression",
    "227-run-on-expression-block-break-option",
    "228-run-core-option-expect",
    "229-run-core-option-expect-abort",
    "231-run-on-pattern-same-name-different-payload",
    "232-run-on-pattern-binder-read-in-condition",
    "233-run-generic-on-pattern-binder-read-in-condition",
    "234-run-on-expression-branch-binder-result",
    "235-run-pointer-dynarray-count-logical",
    "238-run-dynarray-result-pattern",
    "242-run-atomic-operators",
    "243-run-compile-time-parameter",
    "244-run-compound-functions",
    "251-run-atomic-method-name-is-not-intrinsic",
    "252-run-nested-partial-on-statement",
    "254-run-core-done-reports-leaks",
    "255-run-llvm-on-value-intermediate-expressions",
    "258-run-dynamic-array-leak-location",
    "259-run-lazy-arena-leak-location",
    "260-run-direct-recursive-function",
    "261-run-typed-plex-shorthand-before-ellipsis",
    "262-run-addressed-local-in-skipped-on",
    "265-run-byte-slice-equality",
    "266-run-mutable-slice-literal",
    "267-run-fixed-array-equality",
    "270-run-condition-on-in-expression-block",
    "271-run-void-result-success",
    "272-run-core-arena-pr",
    "272-run-negate-optional-result",
    "274-run-assignment-context-local-inference",
    "275-run-imported-default-arena",
    "275-run-range-item-used-in-nested-local",
    "276-run-propagate-call-once",
    "277-run-unused-default-once",
    "278-run-implicit-on-result-binders",
    "281-run-plex-bit-fields",
    "282-run-range-membership",
    "283-run-compound-assignment-inference",
    "284-run-bitfield-enum-display",
    "285-run-pointer-ordering",
    "287-run-inferred-expression-propagation",
    "288-run-usage-context-inference",
    "291-run-nested-tuple-items",
    "292-run-plex-usage-context",
    "294-run-content-equality",
    "294-run-named-trait-parameters",
    "295-run-imported-content-equality",
    "296-run-merged-standard-library",
    "298-run-variadic-promotions",
    "299-run-variadic-forwarding",
    "302-run-public-main",
    "304-run-named-function-table",
    "305-run-format-c-string",
    "306-run-void-result-propagation",
    "307-run-imported-display-interpolation",
    "308-run-shutdown-watcher",
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
    parts = sections(fixture)
    work = tmp / fixture.stem
    work.mkdir()
    source = work / (fixture.stem + ".n")
    source.write_text(parts[0], encoding="utf-8")
    stdin = parts[5].encode() if fixture.suffix == ".t" and len(parts) > 5 else None
    output = work / ("program.exe" if os.name == "nt" else "program")
    run([nerd, "build", "-o", output, source], env)
    expected = run([output], env, check=False, stdin=stdin)
    output.unlink()
    run([nerd, "build", "--genc", "-o", output, source], env)
    assert not output.exists(), "C generation must not create a binary"
    generated = output.with_suffix(".c")
    assert generated.is_file(), "C output must replace the binary extension"
    for optimisation in ("-O0", "-O2"):
        run(["clang", "-std=gnu11", optimisation,
             "-Werror", generated, "-o", output,
             *([] if os.name == "nt" else ["-lm"])], env)
        actual = run([output], env, check=False, stdin=stdin)
        assert behaviour(actual) == behaviour(expected), (
            f"{fixture.name} {optimisation}: C and LLVM behaviour differs\n"
            f"C: {behaviour(actual)!r}\nLLVM: {behaviour(expected)!r}")
    return f"[PASS] C: {fixture.name} (-O0, -O2)"


def cli_checks(nerd, tmp, env):
    work = tmp / "paths with spaces"
    work.mkdir()
    source = work / "hello.n"
    source.write_text('main :: fn () { prn("generated C") }\n', encoding="utf-8")
    # Generation is independent of clang and the runtime object cache.
    no_compiler = {**env, "PATH": str(work / "empty-path")}
    run([nerd, "build", "--genc", "--hir", source], no_compiler)
    assert source.with_suffix(".c").is_file()
    assert list(work.glob("*.hir")), "--genc --hir must preserve the HIR sidecar"
    assert not source.with_suffix(".exe" if os.name == "nt" else "").exists()
    for suffix in ("", ".exe", ".out", ".c"):
        output = work / ("custom" + suffix)
        run([nerd, "build", "--genc", "-o", output, source], env)
        assert output.with_suffix(".c").is_file()
    snippet_output = work / "snippet"
    run([nerd, "build", "--genc", "-o", snippet_output,
         'main :: fn (args: []string) -> i32 { assert args.count == 3\n assert args[1] == "one"\n assert args[2] == "two words"\n return 7 }'], env)
    executable = work / ("snippet.exe" if os.name == "nt" else "snippet")
    run(["clang", snippet_output.with_suffix(".c"), "-o", executable], env)
    result = run([executable, "one", "two words"], env, check=False)
    assert behaviour(result) == (7, b"", b""), "generated main must forward arguments and exit status"
    invalid = work / "invalid.n"
    invalid.write_text("main :: fn () { missing_symbol() }", encoding="utf-8")
    result = run([nerd, "build", "--genc", invalid], env, check=False)
    assert result.returncode and not invalid.with_suffix(".c").exists()


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
    source.write_text((ROOT / "examples/dungeon/dungeon.n").read_text().replace(
        "seed = now()", "seed = 12345"), encoding="utf-8")
    executable = work / "dungeon"
    run([nerd, "build", "-o", executable, source], env)
    expected = terminal_frame(executable, env)
    executable.unlink()
    run([nerd, "build", "--genc", "-o", executable, source], env)
    for optimisation in ("-O0", "-O2"):
        run(["clang", "-Werror", optimisation, source.with_suffix(".c"), "-o", executable], env)
        actual = terminal_frame(executable, env)
        assert actual == expected, f"Dungeon {optimisation}: first frame differs between C and LLVM"
    print("[PASS] Dungeon renders before input, matches LLVM and quits on Q (-O0, -O2)", flush=True)


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
        dungeon_check(nerd, tmp, env)
        for flags in (("--llvm",), ("--obj",), ("--lib",), ("--dll",)):
            result = run([nerd, "build", "--genc", *flags, "main :: fn () {}"], env, check=False)
            assert result.returncode, f"--genc must reject {flags}"
    print(f"C generation: {len(fixtures)} differential fixtures at two optimisation levels and CLI conflicts passed")


if __name__ == "__main__":
    main()
