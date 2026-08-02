use std.atomics

main :: fn () {
    value : atomic[i32] = 0
    value.store(1, Release)
    scratch := arena(4096, 1024)
    _bytes := scratch.alloc_array[u8](32)
    values : [..]i32
    values.reserve_extra(8)
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/hover",
        "params": {
            "textDocument": { "uri": "file:///test.n" },
            "position": { "line": 4, "character": 10 }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "method": "textDocument/signatureHelp",
        "params": {
            "textDocument": { "uri": "file:///test.n" },
            "position": { "line": 4, "character": 26 }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "method": "textDocument/definition",
        "params": {
            "textDocument": { "uri": "file:///test.n" },
            "position": { "line": 4, "character": 22 }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "method": "textDocument/hover",
        "params": {
            "textDocument": { "uri": "file:///test.n" },
            "position": { "line": 5, "character": 16 }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 6,
        "method": "textDocument/signatureHelp",
        "params": {
            "textDocument": { "uri": "file:///test.n" },
            "position": { "line": 5, "character": 31 }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 7,
        "method": "textDocument/hover",
        "params": {
            "textDocument": { "uri": "file:///test.n" },
            "position": { "line": 6, "character": 25 }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 8,
        "method": "textDocument/signatureHelp",
        "params": {
            "textDocument": { "uri": "file:///test.n" },
            "position": { "line": 6, "character": 40 }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 9,
        "method": "textDocument/hover",
        "params": {
            "textDocument": { "uri": "file:///test.n" },
            "position": { "line": 8, "character": 14 }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 10,
        "method": "textDocument/signatureHelp",
        "params": {
            "textDocument": { "uri": "file:///test.n" },
            "position": { "line": 8, "character": 26 }
        }
    }
]
¬
[
    {
        "jsonrpc": "2.0",
        "id": 1,
        "result": {
            "serverInfo": { "name": "Nerd LSP", "version": "0.1.0" },
            "capabilities": {
                "textDocumentSync": { "openClose": true, "change": 2 },
                "hoverProvider": true,
                "definitionProvider": true,
                "documentSymbolProvider": true,
                "completionProvider": {
                    "triggerCharacters": [".", "{"],
                    "resolveProvider": false
                },
                "signatureHelpProvider": {
                    "triggerCharacters": ["(", ","],
                    "retriggerCharacters": [",", "\n"]
                },
                "semanticTokensProvider": {
                    "legend": {
                        "tokenTypes": ["variable", "function", "keyword", "number", "operator", "string"],
                        "tokenModifiers": ["unnecessary"]
                    },
                    "full": true
                }
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "method": "textDocument/publishDiagnostics",
        "params": { "uri": "file:///test.n", "diagnostics": [] }
    },
    {
        "jsonrpc": "2.0",
        "id": 2,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nstore :: fn (value: T, order :: AtomicStoreOrder = SequentiallyConsistent) -> void\n```\n\n- Kind: atomic method\n- Owner: `atomic[i32]`\n\nStores a value using the selected compile-time memory order."
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "signatures": [
                {
                    "label": "store(value: T, order :: AtomicStoreOrder = SequentiallyConsistent) -> void",
                    "documentation": "Stores a value using the selected compile-time memory order.",
                    "parameters": [
                        { "label": [6, 14] },
                        { "label": [16, 66] }
                    ]
                }
            ],
            "activeSignature": 0,
            "activeParameter": 1
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": {
            "uri": "file:///home/matt/nerd/mods/std/atomics.n",
            "range": {
                "start": { "line": 14, "character": 4 },
                "end": { "line": 14, "character": 11 }
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\narena\n```\n\n```nerd\narena :: fn (num_bytes: usize, increment: usize = num_bytes) -> arena\n```\n\n- Kind: built-in type\n- Type: `arena`\n- Notes: opaque, pointer-stable allocation arena\n\nCreates a pointer-stable arena with the requested initial capacity. The optional increment controls how much storage is committed when more space is needed."
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 6,
        "result": {
            "signatures": [
                {
                    "label": "arena(num_bytes: usize, increment: usize = num_bytes) -> arena",
                    "documentation": "Creates a pointer-stable arena with the requested initial capacity. The optional increment controls how much storage is committed when more space is needed.",
                    "parameters": [
                        { "label": [6, 22] },
                        { "label": [24, 52] }
                    ]
                }
            ],
            "activeSignature": 0,
            "activeParameter": 1
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 7,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nalloc_array :: fn [T] (count: usize) -> []T\n```\n\n- Kind: arena method\n- Owner: `arena`\n\nAllocates a contiguous slice of count naturally aligned T values from the arena."
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 8,
        "result": {
            "signatures": [
                {
                    "label": "alloc_array[T](count: usize) -> []T",
                    "documentation": "Allocates a contiguous slice of count naturally aligned T values from the arena.",
                    "parameters": [
                        { "label": [15, 27] }
                    ]
                }
            ],
            "activeSignature": 0,
            "activeParameter": 0
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 9,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nreserve_extra :: fn (additional: usize) -> void\n```\n\n- Kind: dynamic array method\n- Owner: `[..]i32`\n\nEnsures room for the requested number of additional live values."
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 10,
        "result": {
            "signatures": [
                {
                    "label": "reserve_extra(additional: usize) -> void",
                    "documentation": "Ensures room for the requested number of additional live values.",
                    "parameters": [
                        { "label": [14, 31] }
                    ]
                }
            ],
            "activeSignature": 0,
            "activeParameter": 0
        }
    },
    { "jsonrpc": "2.0", "id": 999, "result": null }
]
