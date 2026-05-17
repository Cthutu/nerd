Point :: plex {
    x i32
    y i32
}

main :: fn () {
    point := Point {
    }
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
                    "line": 6,
                    "character": 13
                },
                "end": {
                    "line": 6,
                    "character": 13
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
                            "line": 6,
                            "character": 19
                        },
                        "end": {
                            "line": 6,
                            "character": 20
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
                                        "line": 6,
                                        "character": 19
                                    },
                                    "end": {
                                        "line": 6,
                                        "character": 20
                                    }
                                }
                            },
                            "message": "note: Missing fields: `x`, `y`"
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 6,
                                        "character": 19
                                    },
                                    "end": {
                                        "line": 6,
                                        "character": 20
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
                                        "line": 7,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 7,
                                        "character": 4
                                    }
                                },
                                "newText": "        x: 0\n        y: 0\n    "
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
