from __future__ import annotations

import os
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
SOURCE = ROOT / "data" / "prelude.c"
OUTPUT = ROOT / "_obj" / "llvm" / "prelude.ll"


RUNTIME_FUNCTIONS = [
    "nerd_assert",
    "string_eq",
    "string_builder_append_string",
    "string_builder_finish",
    "to_string$string",
    "to_string$bool",
    "to_string$i8",
    "to_string$i16",
    "to_string$i32",
    "to_string$i64",
    "to_string$u8",
    "to_string$u16",
    "to_string$u32",
    "to_string$u64",
    "to_string$isize",
    "to_string$usize",
    "to_string$f32",
    "to_string$f64",
]


TO_STRING_SIGNATURES = [
    ("to_string$string", "{ ptr, i64 }"),
    ("to_string$bool", "i1"),
    ("to_string$i8", "i8"),
    ("to_string$i16", "i16"),
    ("to_string$i32", "i32"),
    ("to_string$i64", "i64"),
    ("to_string$u8", "i8"),
    ("to_string$u16", "i16"),
    ("to_string$u32", "i32"),
    ("to_string$u64", "i64"),
    ("to_string$isize", "i64"),
    ("to_string$usize", "i64"),
    ("to_string$f32", "float"),
    ("to_string$f64", "double"),
]


def llvm_name(name: str) -> str:
    return f'@"{name}"' if "$" in name else f"@{name}"


def abi_name(name: str) -> str:
    return f"__nerd_abi_{name}"


def rename_windows_abi_functions(text: str) -> str:
    for name in RUNTIME_FUNCTIONS:
        text = text.replace(llvm_name(name), llvm_name(abi_name(name)))
    return text


def append_windows_wrappers(text: str) -> str:
    wrappers = """

define void @nerd_assert(i1 %condition, ptr %source_path, i32 %line, { ptr, i64 } %message) {
  %message.slot = alloca %struct.string, align 8
  store { ptr, i64 } %message, ptr %message.slot, align 8
  call void @__nerd_abi_nerd_assert(i1 %condition, ptr %source_path, i32 %line, ptr %message.slot)
  ret void
}

define i1 @string_eq({ ptr, i64 } %lhs, { ptr, i64 } %rhs) {
  %lhs.slot = alloca %struct.string, align 8
  %rhs.slot = alloca %struct.string, align 8
  store { ptr, i64 } %lhs, ptr %lhs.slot, align 8
  store { ptr, i64 } %rhs, ptr %rhs.slot, align 8
  %result = call i1 @__nerd_abi_string_eq(ptr %lhs.slot, ptr %rhs.slot)
  ret i1 %result
}

define void @string_builder_append_string({ ptr, i64 } %value) {
  %value.slot = alloca %struct.string, align 8
  store { ptr, i64 } %value, ptr %value.slot, align 8
  call void @__nerd_abi_string_builder_append_string(ptr %value.slot)
  ret void
}

define { ptr, i64 } @string_builder_finish(i64 %start) {
  %result.slot = alloca %struct.string, align 8
  call void @__nerd_abi_string_builder_finish(ptr sret(%struct.string) align 8 %result.slot, i64 %start)
  %result = load { ptr, i64 }, ptr %result.slot, align 8
  ret { ptr, i64 } %result
}
"""
    for name, arg_type in TO_STRING_SIGNATURES:
        arg_setup = ""
        call_arg = "%value"
        if arg_type == "{ ptr, i64 }":
            arg_setup = (
                "  %value.slot = alloca %struct.string, align 8\n"
                "  store { ptr, i64 } %value, ptr %value.slot, align 8\n"
            )
            call_arg = "%value.slot"
        wrappers += f"""
define {{ ptr, i64 }} {llvm_name(name)}({arg_type} %value) {{
  %result.slot = alloca %struct.string, align 8
{arg_setup}  call void {llvm_name(abi_name(name))}(ptr sret(%struct.string) align 8 %result.slot, {arg_type if arg_type != "{ ptr, i64 }" else "ptr"} {call_arg})
  %result = load {{ ptr, i64 }}, ptr %result.slot, align 8
  ret {{ ptr, i64 }} %result
}}
"""
    return text.rstrip() + wrappers


def main() -> int:
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    cmd = [
        "clang",
        "-S",
        "-emit-llvm",
        "-O0",
        "-Xclang",
        "-disable-O0-optnone",
        "-std=gnu23",
        str(SOURCE),
        "-o",
        str(OUTPUT),
    ]
    result = subprocess.run(cmd)
    if result.returncode != 0:
        return result.returncode

    if os.name == "nt":
        text = OUTPUT.read_text(encoding="utf-8")
        text = rename_windows_abi_functions(text)
        text = append_windows_wrappers(text)
        OUTPUT.write_text(text, encoding="utf-8", newline="\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
