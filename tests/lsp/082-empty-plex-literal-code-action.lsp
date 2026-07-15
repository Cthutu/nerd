use std.frame

main :: fn () {
    main_frame: Frame = { }
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/codeAction",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "range": {
                "start": {
                    "line": 3,
                    "character": 25
                },
                "end": {
                    "line": 3,
                    "character": 25
                }
            },
            "context": {
                "diagnostics": []
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
                            "character": 24
                        },
                        "end": {
                            "line": 3,
                            "character": 25
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Plex literal is missing required fields",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 3,
                                        "character": 24
                                    },
                                    "end": {
                                        "line": 3,
                                        "character": 25
                                    }
                                }
                            },
                            "message": "note: Missing fields: `system`, `id`, `width`, `height`, `title`, `full_screen`, `resizable`"
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 3,
                                        "character": 24
                                    },
                                    "end": {
                                        "line": 3,
                                        "character": 25
                                    }
                                }
                            },
                            "message": "help: Add all fields required by the plex type, or write `...` in the literal to initialise omitted fields with their default values."
                        }
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
                "title": "Fill missing plex fields",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "file:///test.n": [
                            {
                                "range": {
                                    "start": {
                                        "line": 3,
                                        "character": 25
                                    },
                                    "end": {
                                        "line": 3,
                                        "character": 26
                                    }
                                },
                                "newText": "\n        system     : nil\n        id         : 0\n        width      : 0\n        height     : 0\n        title      : \"\"\n        full_screen: no\n        resizable  : no\n    "
                            }
                        ]
                    }
                }
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
