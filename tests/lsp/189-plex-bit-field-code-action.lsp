Header :: plex {
    u8 {
        version : 4
        _       : 2
        kind    : 2
    }
    length u16
}

main :: fn () {
    _ := Header {
        version: 1
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
                    "line": 11,
                    "character": 8
                },
                "end": {
                    "line": 11,
                    "character": 8
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
                            "line": 10,
                            "character": 16
                        },
                        "end": {
                            "line": 10,
                            "character": 17
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
                                        "line": 10,
                                        "character": 16
                                    },
                                    "end": {
                                        "line": 10,
                                        "character": 17
                                    }
                                }
                            },
                            "message": "note: Missing fields: `kind`, `length`"
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 10,
                                        "character": 16
                                    },
                                    "end": {
                                        "line": 10,
                                        "character": 17
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
                                        "line": 12,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 12,
                                        "character": 4
                                    }
                                },
                                "newText": "\n        kind   : 0\n        length : 0\n    "
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
