#!/usr/bin/env python3
"""Exercise Linux desktop providers without displaying UI or launching applications."""
import argparse
import json
import os
import shlex
import shutil
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def run(command, env):
    result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True,
                            text=True, encoding="utf-8", timeout=30)
    if result.returncode or result.stderr:
        raise AssertionError(f"{command}: {result.returncode}\n{result.stdout}\n{result.stderr}")
    return result.stdout


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--nerd", type=Path, required=True)
    parser.add_argument("--cgen", action="store_true")
    args = parser.parse_args()
    if os.name == "nt":
        print("Linux desktop provider tests skipped on Windows")
        return
    compiler = args.nerd.resolve()
    env = dict(os.environ, NERD_LIB_PATH=str(ROOT / "mods"))
    (ROOT / "_tmp").mkdir(exist_ok=True)
    directory = Path(tempfile.mkdtemp(prefix="stdlib-system-", dir=ROOT / "_tmp"))
    try:
        binary = directory / "provider"
        command = [str(compiler), "build", "-o", str(binary),
                   str(ROOT / "tests/stdlib-system/provider.n")]
        if args.cgen:
            copts = run([*command, "--cgen", "--copts"], env)
            run(["clang", str(binary.with_suffix(".c")), *shlex.split(copts),
                 "-o", str(binary)], env)
        else:
            run(command, env)
        providers = directory / "bin"
        providers.mkdir()
        log = directory / "arguments.jsonl"
        script = "#!" + os.sys.executable + "\n" + r'''
import json, os, sys
with open(os.environ["SYSTEM_TEST_LOG"], "a", encoding="utf-8") as output:
    output.write(json.dumps(sys.argv[1:], ensure_ascii=False) + "\n")
if os.path.basename(sys.argv[0]) == "xdg-open":
    sys.exit(0)
mode = os.environ.get("SYSTEM_TEST_MODE", "ok")
if mode == "cancel": sys.exit(1)
if mode == "fail": sys.exit(5)
if mode == "empty": sys.exit(0)
if mode == "large":
    sys.stdout.write("x" * 1100000 + "\n")
else:
    sys.stdout.write(" /tmp/界 path with spaces \n\n")
'''
        for name in ("zenity", "xdg-open"):
            provider = providers / name
            provider.write_text(script, encoding="utf-8")
            provider.chmod(0o755)
        env.update(PATH=str(providers), SYSTEM_TEST_LOG=str(log))
        for kind in ("file", "save", "folder"):
            assert run([str(binary), kind], env) == "selected:[ /tmp/界 path with spaces \n]\n"
        recorded = [json.loads(line) for line in log.read_text(encoding="utf-8").splitlines()]
        assert recorded == [
            ["--file-selection", '--title=Quote " and $shell; title'],
            ["--file-selection", "--title=Save", "--save", "--confirm-overwrite",
             "--filename=two words;$(unsafe).txt"],
            ["--file-selection", "--title=Select folder", "--directory"],
        ], recorded
        for mode, expected in (("cancel", "cancelled"), ("fail", "failed"),
                               ("empty", "failed"), ("large", "failed")):
            env["SYSTEM_TEST_MODE"] = mode
            assert run([str(binary), "file"], env) == expected + "\n"
        run([str(binary), "open"], env)
        recorded = [json.loads(line) for line in log.read_text(encoding="utf-8").splitlines()]
        assert recorded[-3:] == [["https://example.test/a?x=one&y=two"],
                                  ["./-name with spaces;$(echo unsafe)"], ["./."]]
        (providers / "zenity").unlink()
        assert run([str(binary), "file"], env) == "unavailable\n"
    except BaseException:
        print(f"Desktop provider test evidence retained in {directory}")
        raise
    else:
        shutil.rmtree(directory)
    print("Linux desktop provider arguments, capture, cancellation and failures passed")


if __name__ == "__main__":
    main()
