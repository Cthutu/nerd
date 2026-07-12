use std.atomics

main :: fn () {
    value : atomic[i32] = 0
    value.store(1, Release)
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
                "value": "```nerd\nstore :: fn (self: ^atomic[T], value: T, order :: AtomicStoreOrder = SequentiallyConsistent)\n```\n\n- Kind: method"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "signatures": [
                {
                    "label": "__impl_atomic_T_store(self: ^atomic[T], value: T, order :: AtomicStoreOrder = SequentiallyConsistent)",
                    "documentation": "Named arguments use `name = value`; omitted parameters use declared defaults when available.",
                    "parameters": [
                        { "label": [22, 38] },
                        { "label": [40, 48] },
                        { "label": [50, 100] }
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
    { "jsonrpc": "2.0", "id": 999, "result": null }
]
