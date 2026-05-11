#!/usr/bin/env python3
from __future__ import annotations

import shutil
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[3]
EXPERIMENT = Path(__file__).resolve().parent
OUT = ROOT / "tmp" / "llvm-runtime-bridge"


def run(cmd: list[str]) -> None:
    print("+", " ".join(cmd))
    subprocess.run(cmd, cwd=ROOT, check=True)


def require_tool(name: str) -> None:
    if shutil.which(name) is None:
        raise SystemExit(f"required tool not found: {name}")


def llvm_target_lines(prelude_ll: Path) -> list[str]:
    return [
        line
        for line in prelude_ll.read_text(encoding="utf-8").splitlines()
        if line.startswith("target datalayout = ") or line.startswith(
            "target triple = "
        )
    ]


def write_epilogue_ll(prelude_ll: Path, path: Path) -> None:
    path.write_text(
        '\n'.join(
            [
                *llvm_target_lines(prelude_ll),
                "",
                'declare void @init()',
                'declare i32 @"$main"()',
                "",
                "define i32 @main() {",
                "entry:",
                "  call void @init()",
                '  %result = call i32 @"$main"()',
                "  ret i32 %result",
                "}",
            ]
        ),
        encoding="utf-8",
    )


def write_targeted_program_ll(prelude_ll: Path,
                              source_ll: Path,
                              output_ll: Path) -> None:
    body_lines = [
        line
        for line in source_ll.read_text(encoding="utf-8").splitlines()
        if not line.startswith("target datalayout = ")
        and not line.startswith("target triple = ")
    ]
    output_ll.write_text(
        "\n".join([*llvm_target_lines(prelude_ll), "", *body_lines, ""]),
        encoding="utf-8",
    )


def main() -> int:
    require_tool("clang")
    require_tool("llvm-link")

    OUT.mkdir(parents=True, exist_ok=True)

    common_c_flags = [
        "clang",
        "-std=gnu23",
        "-Wno-dollar-in-identifier-extension",
        "-S",
        "-emit-llvm",
    ]

    prelude_ll = OUT / "prelude.ll"
    epilogue_ll = OUT / "epilogue.ll"
    program_template_ll = EXPERIMENT / "program.ll"
    program_ll = OUT / "program.ll"
    linked_bc = OUT / "linked.bc"
    exe = OUT / "runtime-bridge"

    run([
        *common_c_flags,
        str(ROOT / "data" / "prelude.c"),
        "-o",
        str(prelude_ll),
    ])
    write_epilogue_ll(prelude_ll, epilogue_ll)
    write_targeted_program_ll(prelude_ll, program_template_ll, program_ll)
    run([
        "llvm-link",
        str(prelude_ll),
        str(epilogue_ll),
        str(program_ll),
        "-o",
        str(linked_bc),
    ])
    run(["clang", str(linked_bc), "-o", str(exe)])
    run([str(exe)])

    print(f"ok: {exe.relative_to(ROOT)} exited successfully")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
