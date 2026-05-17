Display :: trait {
    show :: fn (Self) -> string
}

main :: fn () => 0
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
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "method": "textDocument/semanticTokens/full",
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
                        "tokenModifiers": []
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
                "name": "Display",
                "kind": 11,
                "range": {
                    "start": {
                        "line": 0,
                        "character": 0
                    },
                    "end": {
                        "line": 0,
                        "character": 7
                    }
                },
                "selectionRange": {
                    "start": {
                        "line": 0,
                        "character": 0
                    },
                    "end": {
                        "line": 0,
                        "character": 7
                    }
                },
                "detail": "trait"
            },
            {
                "name": "main",
                "kind": 12,
                "range": {
                    "start": {
                        "line": 4,
                        "character": 0
                    },
                    "end": {
                        "line": 4,
                        "character": 4
                    }
                },
                "selectionRange": {
                    "start": {
                        "line": 4,
                        "character": 0
                    },
                    "end": {
                        "line": 4,
                        "character": 4
                    }
                },
                "detail": "function"
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "data": [
                0,
                0,
                7,
                0,
                0,
                0,
                8,
                1,
                4,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                2,
                5,
                2,
                0,
                1,
                4,
                4,
                0,
                0,
                0,
                5,
                1,
                4,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                2,
                2,
                2,
                0,
                0,
                4,
                4,
                0,
                0,
                0,
                6,
                2,
                4,
                0,
                0,
                3,
                6,
                0,
                0,
                3,
                0,
                4,
                1,
                0,
                0,
                5,
                1,
                4,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                2,
                2,
                2,
                0,
                0,
                6,
                2,
                4,
                0,
                0,
                3,
                1,
                3,
                0
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
