"""Check examples directly from the learning guide, without source copies."""

import argparse
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]
LEARN = ROOT / "docs" / "learn"
ANSI = re.compile(r"\x1b\[[0-9;]*m")


def extract(case: dict) -> str:
    text = (LEARN / case["chapter"]).read_text(encoding="utf-8")
    sections = re.split(r"^## (.+)\n", text, flags=re.M)
    matches = [sections[i + 1] for i in range(1, len(sections), 2)
               if sections[i] == case["section"]]
    if len(matches) != 1:
        raise ValueError(f"Missing or ambiguous section: {case['section']}")
    blocks = re.findall(r"^```nerd\n(.*?)^```", matches[0], re.M | re.S)
    return blocks[case["block"] - 1]


def check_links() -> None:
    for source in LEARN.glob("*.md"):
        prose = re.sub(r"^```.*?^```", "", source.read_text(encoding="utf-8"), flags=re.M | re.S)
        prose = re.sub(r"`[^`]*`", "", prose)
        for target in re.findall(r"\]\(([^)]+)\)", prose):
            path = target.split("#", 1)[0]
            if path and not re.match(r"[a-z]+:", path):
                if not (source.parent / path).exists():
                    raise ValueError(f"Broken link in {source.name}: {target}")


def check_case(case: dict, folder: Path, compiler: Path) -> None:
    code = extract(case)
    command = case["command"]
    if command != "artifacts" and not re.search(r"\bmain\s*::", code):
        code += "\nmain :: fn () {}\n"
    source = folder / "main.n"
    source.write_text(code, encoding="utf-8")
    (folder / "message.txt").write_text("Hello from an asset!", encoding="utf-8")
    (folder / "greeting.n").write_text(
        'pub greet :: fn (name: string) { prn($"Hello, {name}!") }\n',
        encoding="utf-8",
    )
    env = os.environ.copy()
    env["NERD_LIB_PATH"] = str(ROOT / "mods")

    def run(args: list[str], status: int = 0, stdout: str | None = "") -> None:
        result = subprocess.run(args, cwd=folder, env=env, text=True,
                                capture_output=True, timeout=120)
        actual = ANSI.sub("", result.stdout).replace("\r\n", "\n")
        expected = None if stdout is None else stdout.replace("{source}", str(source))
        expected = None if expected is None else expected.replace("{executable}", str(executable))
        if result.returncode != status or result.stderr or (expected is not None and actual != expected):
            raise RuntimeError(
                f"{args}\nExpected status {status}, stdout {expected!r}; "
                f"got {result.returncode}, stdout {actual!r}, stderr {result.stderr!r}"
            )

    executable = folder / ("program.exe" if os.name == "nt" else "program")
    if command == "artifacts":
        for flag, suffix in [("--obj", ".obj" if os.name == "nt" else ".o"),
                             ("--lib", ".lib" if os.name == "nt" else ".a"),
                             ("--dll", ".dll" if os.name == "nt" else ".so")]:
            output = folder / ("arithmetic" + suffix)
            run([str(compiler), "build", flag, "-o", str(output), str(source)])
            if not output.is_file() or output.stat().st_size == 0:
                raise RuntimeError(f"Missing or empty artifact: {output}")
    elif command == "run":
        run([str(compiler), "build", "-o", str(executable), str(source)])
        run([str(executable), *case.get("args", [])], case["status"], case["stdout"])
    else:
        run([str(compiler), command, str(source)], case.get("status", 0), case.get("stdout", ""))


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--nerd", default="_bin/nerd-debug.exe" if os.name == "nt" else "_bin/nerd-debug")
    args = parser.parse_args()
    compiler = Path(args.nerd).resolve()
    check_links()
    cases = json.loads((ROOT / "tests/docs/cases.json").read_text(encoding="utf-8"))
    folder = Path(tempfile.mkdtemp(prefix="nerd-docs-")).resolve()
    failures = 0
    for i, case in enumerate(cases):
        work = folder / str(i)
        work.mkdir()
        name = f"{case['chapter']}: {case['section']} ({case['block']})"
        try:
            check_case(case, work, compiler)
            print(f"[PASS] {name}", flush=True)
        except (ValueError, IndexError, RuntimeError, subprocess.TimeoutExpired) as error:
            failures += 1
            print(f"[FAIL] {name}: {error}", flush=True)
    if failures:
        print(f"{failures} documentation checks failed; evidence retained in {folder}")
        return 1
    shutil.rmtree(folder)
    print(f"{len(cases)} documentation examples passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
