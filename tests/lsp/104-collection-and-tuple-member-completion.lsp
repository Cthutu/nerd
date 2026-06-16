main :: fn () {
    pair: (i32, string) = (1, "one")
    pair_ptr := ^pair
    values: [3]i32 = [1, 2, 3]
    slice := values[..]
    slice_ptr := ^slice
    _ := pair_ptr.0
    _ := values.count
    _ := slice_ptr.count
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/completion",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 6,
                "character": 18
            },
            "context": {
                "triggerKind": 2,
                "triggerCharacter": "."
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "method": "textDocument/completion",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 7,
                "character": 16
            },
            "context": {
                "triggerKind": 2,
                "triggerCharacter": "."
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "method": "textDocument/completion",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 8,
                "character": 19
            },
            "context": {
                "triggerKind": 2,
                "triggerCharacter": "."
            }
        }
    }
]
¬
[
    {
        "jsonrpc": "2.0",
        "id": 1,
        "result": {
            "serverInfo": {
                "name": "Nerd LSP",
                "version": "0.1.0"
            },
            "capabilities": {
                "textDocumentSync": {
                    "openClose": true,
                    "change": 2
                },
                "hoverProvider": true,
                "definitionProvider": true,
                "documentSymbolProvider": true,
                "completionProvider": {
                    "triggerCharacters": [
                        ".",
                        "{"
                    ],
                    "resolveProvider": false
                },
                "signatureHelpProvider": {
                    "triggerCharacters": [
                        "(",
                        ","
                    ],
                    "retriggerCharacters": [
                        ",",
                        "\n"
                    ]
                },
                "semanticTokensProvider": {
                    "legend": {
                        "tokenTypes": [
                            "variable",
                            "function",
                            "keyword",
                            "number",
                            "operator",
                            "string"
                        ],
                        "tokenModifiers": [
                            "unnecessary"
                        ]
                    },
                    "full": true
                }
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "method": "textDocument/publishDiagnostics",
        "params": {
            "uri": "file:///test.n",
            "diagnostics": []
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 2,
        "result": [
            {
                "label": "0",
                "kind": 5,
                "detail": "field"
            },
            {
                "label": "1",
                "kind": 5,
                "detail": "field"
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": [
            {
                "label": "count",
                "kind": 5,
                "detail": "field"
            },
            {
                "label": "size",
                "kind": 5,
                "detail": "field"
            },
            {
                "label": "bytes",
                "kind": 5,
                "detail": "field"
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": [
            {
                "label": "data",
                "kind": 5,
                "detail": "field"
            },
            {
                "label": "count",
                "kind": 5,
                "detail": "field"
            },
            {
                "label": "size",
                "kind": 5,
                "detail": "field"
            },
            {
                "label": "bytes",
                "kind": 5,
                "detail": "field"
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
