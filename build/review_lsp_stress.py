#!/usr/bin/env python3
from __future__ import annotations

import json
import os
import pathlib
import subprocess
import sys
from dataclasses import dataclass
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[1]
NERD = ROOT / "_bin" / ("nerd-debug.exe" if os.name == "nt" else "nerd-debug")
URI = "file:///stress.n"


@dataclass
class StressCase:
    name: str
    source: str
    messages: list[dict[str, Any]]
    required_response_ids: set[int]


def lsp_frame(payload: dict[str, Any]) -> bytes:
    data = json.dumps(payload).encode()
    return f"Content-Length: {len(data)}\r\n\r\n".encode() + data


def lsp_read_frames(data: bytes) -> list[dict[str, Any]]:
    frames: list[dict[str, Any]] = []
    offset = 0
    while offset < len(data):
        header_end = data.find(b"\r\n\r\n", offset)
        if header_end < 0:
            raise ValueError(f"missing frame header at byte {offset}")
        header = data[offset:header_end].decode()
        length = 0
        for line in header.split("\r\n"):
            if line.lower().startswith("content-length:"):
                length = int(line.split(":", 1)[1].strip())
        if length <= 0:
            raise ValueError(f"invalid frame length at byte {offset}")
        start = header_end + 4
        end = start + length
        if end > len(data):
            raise ValueError("truncated LSP frame")
        frames.append(json.loads(data[start:end]))
        offset = end
    return frames


def pos(line: int, character: int) -> dict[str, int]:
    return {"line": line, "character": character}


def rng(sl: int, sc: int, el: int, ec: int) -> dict[str, dict[str, int]]:
    return {"start": pos(sl, sc), "end": pos(el, ec)}


def did_change(version: int, text: str, range_: dict[str, Any] | None = None) -> dict[str, Any]:
    change: dict[str, Any] = {"text": text}
    if range_ is not None:
        change["range"] = range_
    return {
        "jsonrpc": "2.0",
        "method": "textDocument/didChange",
        "params": {
            "textDocument": {"uri": URI, "version": version},
            "contentChanges": [change],
        },
    }


def request(id_: int, method: str, params: dict[str, Any]) -> dict[str, Any]:
    return {"jsonrpc": "2.0", "id": id_, "method": method, "params": params}


def text_document_params(extra: dict[str, Any] | None = None) -> dict[str, Any]:
    params: dict[str, Any] = {"textDocument": {"uri": URI}}
    if extra:
        params.update(extra)
    return params


BASE_FRAME_SOURCE = """Frame :: plex {
    handle u64
    system ^FrameSystem
}

FrameSystem :: plex {
    frames [..]Frame
}

done :: fn (frame: ^Frame) {
    _ := frame.handle
}
"""


BROKEN_USE_SOURCE = """std :: use std.io

main :: fn () {
    std.prn("ok")
}
"""


BROKEN_TYPE_SOURCE = """Box :: plex [T] {
    value T
}

main :: fn () {
    value: Box[i32]
    _ := value.value
}
"""


def member_access_cases() -> list[StressCase]:
    edit = did_change(2, "", rng(10, 15, 10, 21))
    return [
        StressCase(
            name="completion after member field delete",
            source=BASE_FRAME_SOURCE,
            messages=[
                edit,
                request(
                    2,
                    "textDocument/completion",
                    text_document_params(
                        {
                            "position": pos(10, 15),
                            "context": {
                                "triggerKind": 2,
                                "triggerCharacter": ".",
                            },
                        }
                    ),
                ),
            ],
            required_response_ids={2},
        ),
        StressCase(
            name="hover after member field delete",
            source=BASE_FRAME_SOURCE,
            messages=[
                edit,
                request(
                    2,
                    "textDocument/hover",
                    text_document_params({"position": pos(10, 10)}),
                ),
            ],
            required_response_ids={2},
        ),
        StressCase(
            name="semantic tokens after member field delete",
            source=BASE_FRAME_SOURCE,
            messages=[
                edit,
                request(2, "textDocument/semanticTokens/full", text_document_params()),
            ],
            required_response_ids={2},
        ),
        StressCase(
            name="document symbols after member field delete",
            source=BASE_FRAME_SOURCE,
            messages=[
                edit,
                request(2, "textDocument/documentSymbol", text_document_params()),
            ],
            required_response_ids={2},
        ),
        StressCase(
            name="rename after member field delete",
            source=BASE_FRAME_SOURCE,
            messages=[
                edit,
                request(
                    2,
                    "textDocument/prepareRename",
                    text_document_params({"position": pos(10, 10)}),
                ),
                request(
                    3,
                    "textDocument/rename",
                    text_document_params(
                        {"position": pos(10, 10), "newName": "item"}
                    ),
                ),
            ],
            required_response_ids={2, 3},
        ),
    ]


def module_and_type_cases() -> list[StressCase]:
    return [
        StressCase(
            name="module completion after broken use edit",
            source=BROKEN_USE_SOURCE,
            messages=[
                did_change(2, "", rng(0, 15, 0, 17)),
                request(
                    2,
                    "textDocument/completion",
                    text_document_params(
                        {
                            "position": pos(0, 15),
                            "context": {
                                "triggerKind": 2,
                                "triggerCharacter": ".",
                            },
                        }
                    ),
                ),
            ],
            required_response_ids={2},
        ),
        StressCase(
            name="semantic tokens after broken generic edit",
            source=BROKEN_TYPE_SOURCE,
            messages=[
                did_change(2, "", rng(5, 15, 5, 19)),
                request(2, "textDocument/semanticTokens/full", text_document_params()),
            ],
            required_response_ids={2},
        ),
        StressCase(
            name="hover after broken generic edit",
            source=BROKEN_TYPE_SOURCE,
            messages=[
                did_change(2, "", rng(5, 15, 5, 19)),
                request(
                    2,
                    "textDocument/hover",
                    text_document_params({"position": pos(6, 11)}),
                ),
            ],
            required_response_ids={2},
        ),
    ]


def cases() -> list[StressCase]:
    return [*member_access_cases(), *module_and_type_cases()]


def run_case(case: StressCase) -> list[str]:
    messages = [
        {
            "jsonrpc": "2.0",
            "id": 1,
            "method": "initialize",
            "params": {"rootUri": None, "capabilities": {}},
        },
        {
            "jsonrpc": "2.0",
            "method": "textDocument/didOpen",
            "params": {
                "textDocument": {
                    "uri": URI,
                    "languageId": "nerd",
                    "version": 1,
                    "text": case.source,
                }
            },
        },
        *case.messages,
        {"jsonrpc": "2.0", "id": 999, "method": "shutdown", "params": None},
        {"jsonrpc": "2.0", "method": "exit", "params": None},
    ]
    input_bytes = b"".join(lsp_frame(message) for message in messages)
    proc = subprocess.run(
        [str(NERD), "lsp"],
        cwd=ROOT,
        input=input_bytes,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        timeout=10,
    )

    failures: list[str] = []
    if proc.returncode != 0:
        failures.append(
            f"LSP exited with {proc.returncode}: "
            f"{proc.stderr.decode(errors='replace')}"
        )
        return failures

    try:
        frames = lsp_read_frames(proc.stdout)
    except Exception as exc:  # noqa: BLE001
        failures.append(f"failed to parse LSP frames: {exc}")
        return failures

    response_ids = {
        frame.get("id")
        for frame in frames
        if isinstance(frame, dict) and "id" in frame
    }
    missing = {1, 999, *case.required_response_ids} - response_ids
    if missing:
        failures.append(f"missing response ids: {sorted(missing)}")

    errors = [
        frame
        for frame in frames
        if isinstance(frame, dict) and "id" in frame and "error" in frame
    ]
    if errors:
        failures.append(f"unexpected LSP error response: {errors!r}")

    return failures


def main() -> int:
    if not NERD.exists():
        print(f"Missing {NERD}; run `just build nerd` first.", file=sys.stderr)
        return 2

    failed = False
    for case in cases():
        failures = run_case(case)
        if failures:
            failed = True
            print(f"[FAIL] {case.name}")
            for failure in failures:
                print(f"  {failure}")
        else:
            print(f"[PASS] {case.name}")
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
