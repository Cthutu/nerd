on "debug" {
    answer :: 7

    ffi "c" {
        pub abs (i32) -> i32
    }
}

main :: fn () => answer
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/documentSymbol",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
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
                "name": "answer",
                "kind": 14,
                "range": {
                    "start": {
                        "line": 1,
                        "character": 4
                    },
                    "end": {
                        "line": 1,
                        "character": 10
                    }
                },
                "selectionRange": {
                    "start": {
                        "line": 1,
                        "character": 4
                    },
                    "end": {
                        "line": 1,
                        "character": 10
                    }
                },
                "detail": "constant = 7"
            },
            {
                "name": "abs",
                "kind": 12,
                "range": {
                    "start": {
                        "line": 4,
                        "character": 12
                    },
                    "end": {
                        "line": 4,
                        "character": 15
                    }
                },
                "selectionRange": {
                    "start": {
                        "line": 4,
                        "character": 12
                    },
                    "end": {
                        "line": 4,
                        "character": 15
                    }
                },
                "detail": "function"
            },
            {
                "name": "main",
                "kind": 12,
                "range": {
                    "start": {
                        "line": 8,
                        "character": 0
                    },
                    "end": {
                        "line": 8,
                        "character": 4
                    }
                },
                "selectionRange": {
                    "start": {
                        "line": 8,
                        "character": 0
                    },
                    "end": {
                        "line": 8,
                        "character": 4
                    }
                },
                "detail": "function"
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
