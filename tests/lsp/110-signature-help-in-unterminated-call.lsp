add :: fn (a: i32, b: i32 = 1, c: i32 = a + b) => a + b + c
main :: fn () {
    add(
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/signatureHelp",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 2,
                "character": 8
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
            "diagnostics": [
                {
                    "range": {
                        "start": {
                            "line": 3,
                            "character": 0
                        },
                        "end": {
                            "line": 3,
                            "character": 1
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Missing value before RightBrace `}`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 3,
                                        "character": 0
                                    },
                                    "end": {
                                        "line": 3,
                                        "character": 1
                                    }
                                }
                            },
                            "message": "help: Insert a literal, parenthesized expression, or unary operator"
                        }
                    ]
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 2,
        "result": {
            "signatures": [
                {
                    "label": "add(a: i32, b: i32 = 1, c: i32 = a + b)",
                    "documentation": "Named arguments use `name = value`; omitted parameters use declared defaults when available.",
                    "parameters": [
                        {
                            "label": [
                                4,
                                10
                            ]
                        },
                        {
                            "label": [
                                12,
                                22
                            ]
                        },
                        {
                            "label": [
                                24,
                                38
                            ]
                        }
                    ]
                }
            ],
            "activeSignature": 0,
            "activeParameter": 0
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
