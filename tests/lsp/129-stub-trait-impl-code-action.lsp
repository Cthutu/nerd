Display :: trait {
    show :: fn (Self) -> string
    reset :: fn (Self)
}

Point :: plex {
    x i32
}

impl Display for Point {
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
                    "line": 10,
                    "character": 0
                },
                "end": {
                    "line": 10,
                    "character": 0
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
                            "line": 9,
                            "character": 0
                        },
                        "end": {
                            "line": 9,
                            "character": 4
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Trait implementation is missing required members",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 9,
                                        "character": 0
                                    },
                                    "end": {
                                        "line": 9,
                                        "character": 4
                                    }
                                }
                            },
                            "message": "note: Missing members: `show`, `reset`"
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 9,
                                        "character": 0
                                    },
                                    "end": {
                                        "line": 9,
                                        "character": 4
                                    }
                                }
                            },
                            "message": "help: Add the missing members to this `impl` block."
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
                "title": "Stub missing trait members",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "file:///test.n": [
                            {
                                "range": {
                                    "start": {
                                        "line": 10,
                                        "character": 0
                                    },
                                    "end": {
                                        "line": 10,
                                        "character": 0
                                    }
                                },
                                "newText": "    show :: fn (_self: Self) -> string {\n        return undefined\n    }\n\n    reset :: fn (_self: Self) {\n    }\n"
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
