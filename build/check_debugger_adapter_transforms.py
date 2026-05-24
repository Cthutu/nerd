#!/usr/bin/env python3
import json
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def fail(message: str, detail: str = "") -> int:
    print(f"debugger-adapter-transforms failed: {message}", file=sys.stderr)
    if detail:
        print(detail, file=sys.stderr)
    return 1


def main() -> int:
    compile_result = subprocess.run(
        ["npm", "run", "compile"],
        cwd=ROOT / "syntax" / "nerd-vscode",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    if compile_result.returncode != 0:
        return fail("VS Code extension TypeScript compile failed", compile_result.stdout + compile_result.stderr)

    script = r"""
const fmt = require("./syntax/nerd-vscode/out/debugFormat.js");
const text = `
DebugEvent :: enum {
    None
    Move(DebugPoint)
    Message(string)
    Count(i32) = 7
}

bytes : [..]u8
cells : [..]DebugCell
`;
const arrays = new Set();
const enums = new Map();
fmt.collectDynamicArrayDeclarationsFromText(text, arrays);
fmt.collectEnumDeclarationsFromText(text, enums);
const event = enums.get("DebugEvent");
const result = {
  arrays: Array.from(arrays).sort(),
  variants: event.variants,
  tagHex: fmt.parseIntegerResult("'\\x07'"),
  tagText: fmt.enumTagFromVariableValue("(tag = '\\x07', payload = 0)"),
  parsedTagText: fmt.parseIntegerResult("'\\x01'"),
  tagZero: fmt.enumTagFromVariableValue("(tag = '\\0', payload = 0)"),
  tagColon: fmt.enumTagFromVariableValue("{tag:0, payload: 0}"),
  summary: fmt.enumSummary(event, 7),
  zeroSummary: fmt.enumSummary(event, 0),
  payload: fmt.enumPayloadExpression("event", event.variants[3]),
  pointerType: fmt.nerdDisplayTypeName("unsigned char *"),
  primitiveType: fmt.nerdPrimitiveTypeName("unsigned int"),
};
console.log(JSON.stringify(result));
"""
    node_result = subprocess.run(
        ["node", "-e", script],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    if node_result.returncode != 0:
        return fail("debug formatter transform check failed", node_result.stdout + node_result.stderr)

    result = json.loads(node_result.stdout)
    expected = {
        "arrays": ["bytes", "cells"],
        "tagHex": 7,
        "tagText": 7,
        "parsedTagText": 1,
        "tagZero": 0,
        "tagColon": 0,
        "summary": "DebugEvent.Count (7)",
        "zeroSummary": "DebugEvent.None (0)",
        "payload": "*(int *)((char *)&event.payload + ((sizeof(int) <= 8) ? 8 : 0))",
        "pointerType": "^u8",
        "primitiveType": "u32",
    }
    for key, value in expected.items():
        if result.get(key) != value:
            return fail(f"unexpected {key}", json.dumps(result, indent=2))

    variants = result.get("variants")
    if not variants or variants[1].get("payloadType") != "DebugPoint":
        return fail("enum payload metadata was not collected", json.dumps(result, indent=2))

    print("debugger-adapter-transforms ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
