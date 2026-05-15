#!/usr/bin/env python3
import argparse
import pathlib
import subprocess
import sys
import tempfile
import textwrap


ROOT = pathlib.Path(__file__).resolve().parents[1]


CASES = [
    {
        "name": "ffi-function-continuation",
        "lines": [
            "    ffi user32 {",
            "        pub DefWindowProcA (hWnd   : HWND,",
            "                            msg    : UINT,",
            "                            wParam : WPARAM,",
            "                            lParam : LPARAM) -> LRESULT",
            "",
            "    }",
        ],
        "checks": {3: 28, 6: 8, 7: 4},
    },
    {
        "name": "call-continuation",
        "lines": [
            "main :: fn () {",
            "    value := call(foo,",
            "                  bar)",
            "",
            "}",
        ],
        "checks": {3: 18, 4: 4, 5: 0},
    },
    {
        "name": "block-and-plex",
        "lines": [
            "main :: fn () {",
            "    wndclass :: WNDCLASSA {",
            "        style: 0",
            "    }",
            "}",
        ],
        "checks": {2: 4, 3: 8, 4: 4, 5: 0},
    },
]


def vim_string(value: str) -> str:
    return "'" + value.replace("'", "''") + "'"


def run_case(nvim: str, case: dict[str, object]) -> list[str]:
    lines = case["lines"]
    checks = case["checks"]
    assert isinstance(lines, list)
    assert isinstance(checks, dict)

    vim_lines = ", ".join(vim_string(line) for line in lines)
    check_lines = []
    for line_number, expected in checks.items():
        check_lines.append(
            textwrap.dedent(
                f"""
                let actual = GetNerdIndent({line_number})
                if actual != {expected}
                  call add(g:failures, '{case["name"]}:{line_number}: expected {expected}, got ' . actual)
                endif
                """
            ).strip()
        )
    checks_script = "\n".join(check_lines)
    runtime_path = (ROOT / "syntax" / "nerd-nvim").as_posix()

    script = textwrap.dedent(
        f"""
        set nomore
        set runtimepath^={runtime_path}
        set ft=nerd
        let g:failures = []
        call setline(1, [{vim_lines}])
        {checks_script}
        if !empty(g:failures)
          for failure in g:failures
            echom failure
          endfor
          cquit
        endif
        qa!
        """
    )

    with tempfile.NamedTemporaryFile("w", suffix=".vim", delete=False, encoding="utf-8") as f:
        f.write(script)
        script_path = pathlib.Path(f.name)

    try:
        proc = subprocess.run(
            [nvim, "-n", "--headless", "--clean", "-S", str(script_path)],
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            cwd=ROOT,
        )
    finally:
        script_path.unlink(missing_ok=True)

    if proc.returncode == 0:
        return []
    return [line for line in proc.stdout.splitlines() if line.strip()]


def main() -> int:
    parser = argparse.ArgumentParser(description="Check Nerd Neovim indentation")
    parser.add_argument("--nvim", default="nvim", help="Neovim executable")
    args = parser.parse_args()

    failures: list[str] = []
    for case in CASES:
        failures.extend(run_case(args.nvim, case))

    if failures:
        print("Neovim indentation checks failed:", file=sys.stderr)
        for failure in failures:
            print(f"  {failure}", file=sys.stderr)
        return 1

    print("Neovim indentation checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
