#!/usr/bin/env python3
"""Check UTF-16 incremental edit positions against UTF-8 document storage."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def frame(message: dict) -> bytes:
    body = json.dumps(message, ensure_ascii=False).encode("utf-8")
    return f"Content-Length: {len(body)}\r\n\r\n".encode() + body


def read_frames(data: bytes) -> list[dict]:
    messages = []
    while data:
        header, data = data.split(b"\r\n\r\n", 1)
        length = int(header.split(b":", 1)[1])
        messages.append(json.loads(data[:length]))
        data = data[length:]
    return messages


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--nerd", type=Path, default=ROOT / "_bin" /
                        ("nerd-debug.exe" if os.name == "nt" else "nerd-debug"))
    args = parser.parse_args()
    messages = [{"jsonrpc": "2.0", "id": 1, "method": "initialize",
                 "params": {"rootUri": ROOT.as_uri(), "capabilities": {}}},
                {"jsonrpc": "2.0", "method": "initialized", "params": {}}]
    expected = {}
    cases = [
        ("smart-quote", "-- it\u2019s fine", "\n", False),
        ("multibyte", "-- caf\u00e9 \u6f22\u5b57", "\n", False),
        ("surrogate-pair", "-- \U0001f600 end", "\n", False),
        ("crlf", "-- caf\u00e9 \U0001f600 end", "\r\n", False),
        ("clamp-line-end", "-- plain comment", "\n", True),
        ("clamp-crlf", "-- plain comment", "\r\n", True),
    ]
    for name, comment, newline, clamp in cases:
        uri = (ROOT / "_tmp" / f"lsp-unicode-{name}.n").as_uri()
        source = f"main :: fn () {{{newline}{comment}{newline}}}{newline}"
        messages.append({"jsonrpc": "2.0", "method": "textDocument/didOpen",
                         "params": {"textDocument": {"uri": uri, "languageId": "nerd",
                                                     "version": 1, "text": source}}})
        end = 999 if clamp else len(comment.encode("utf-16-le")) // 2
        # Replace the whole Unicode comment, then delete the new ASCII comment
        # within the same notification. Neither edit may eat the following brace.
        changes = [
            {"range": {"start": {"line": 1, "character": 0},
                       "end": {"line": 1, "character": end}}, "text": "-- x"},
            {"range": {"start": {"line": 1, "character": 0},
                       "end": {"line": 1, "character": 4}}, "text": ""},
        ]
        messages.append({"jsonrpc": "2.0", "method": "textDocument/didChange",
                         "params": {"textDocument": {"uri": uri, "version": 2},
                                    "contentChanges": changes}})
        messages.append({"jsonrpc": "2.0", "method": "textDocument/didClose",
                         "params": {"textDocument": {"uri": uri}}})
        expected[uri] = 3  # open, change, close
    messages.extend([
        {"jsonrpc": "2.0", "id": 2, "method": "shutdown", "params": None},
        {"jsonrpc": "2.0", "method": "exit", "params": None},
    ])
    result = subprocess.run([str(args.nerd.resolve()), "lsp"], cwd=ROOT,
                            input=b"".join(frame(m) for m in messages),
                            capture_output=True, timeout=30, check=True)
    counts = dict.fromkeys(expected, 0)
    for message in read_frames(result.stdout):
        if message.get("method") != "textDocument/publishDiagnostics":
            continue
        params = message["params"]
        uri = params["uri"]
        assert uri in expected, params
        assert not params["diagnostics"], params
        counts[uri] += 1
    assert counts == expected, counts
    print(f"LSP Unicode sync: {len(cases)} cases passed")


if __name__ == "__main__":
    main()
