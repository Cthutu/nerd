use std.io

answer :: 42

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
            "diagnostics": [
                {
                    "range": {
                        "start": {
                            "line": 0,
                            "character": 4
                        },
                        "end": {
                            "line": 0,
                            "character": 10
                        }
                    },
                    "severity": 4,
                    "source": "nerd",
                    "message": "Unused use `std.io`",
                    "tags": [
                        1
                    ]
                }
            ]
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
                        "line": 2,
                        "character": 0
                    },
                    "end": {
                        "line": 2,
                        "character": 6
                    }
                },
                "selectionRange": {
                    "start": {
                        "line": 2,
                        "character": 0
                    },
                    "end": {
                        "line": 2,
                        "character": 6
                    }
                },
                "detail": "constant = 42"
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
        "id": 999,
        "result": null
    }
]
