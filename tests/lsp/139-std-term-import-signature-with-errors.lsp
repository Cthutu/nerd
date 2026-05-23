use std.term

main :: fn () {
    missing()
    term_fb_box()
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
                            "line": 3,
                            "character": 4
                        },
                        "end": {
                            "line": 3,
                            "character": 11
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Unknown symbol `missing`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 3,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 3,
                                        "character": 11
                                    }
                                }
                            },
                            "message": "help: Add a binding for `missing` or fix the spelling."
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
                    "label": "term_fb_box(x: i32, y: i32, w: u32, h: u32, style: BoxStyle = BoxStyle.Single, ink: u32 = COLOUR_TRANSPARENT, paper: u32 = COLOUR_TRANSPARENT) -> void",
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
                                108
                            ]
                        },
                        {
                            "label": [
                                110,
                                141
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
