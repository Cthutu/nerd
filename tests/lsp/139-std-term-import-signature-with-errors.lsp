use std.term

main :: fn () {
    missing()
    term_fb_box(
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
                "line": 4,
                "character": 16
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
                            "line": 5,
                            "character": 0
                        },
                        "end": {
                            "line": 5,
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
                                        "line": 5,
                                        "character": 0
                                    },
                                    "end": {
                                        "line": 5,
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
                    "label": "term_fb_box(x: i32, y: i32, w: i32, h: i32, style: BoxStyle = BoxStyle.Single, ink: Colour = COLOUR_TRANSPARENT, paper: Colour = COLOUR_TRANSPARENT)",
                    "documentation": "Named arguments use `name = value`; omitted parameters use declared defaults when available.",
                    "parameters": [
                        {
                            "label": [
                                12,
                                18
                            ]
                        },
                        {
                            "label": [
                                20,
                                26
                            ]
                        },
                        {
                            "label": [
                                28,
                                34
                            ]
                        },
                        {
                            "label": [
                                36,
                                42
                            ]
                        },
                        {
                            "label": [
                                44,
                                77
                            ]
                        },
                        {
                            "label": [
                                79,
                                111
                            ]
                        },
                        {
                            "label": [
                                113,
                                147
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
