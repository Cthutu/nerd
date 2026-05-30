bad : fn () -> i32 {
    return 1
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
                    "line": 0,
                    "character": 4
                },
                "end": {
                    "line": 0,
                    "character": 5
                }
            },
            "context": {
                "diagnostics": [
                    {
                        "range": {
                            "start": {
                                "line": 0,
                                "character": 19
                            },
                            "end": {
                                "line": 0,
                                "character": 20
                            }
                        },
                        "severity": 1,
                        "source": "nerd",
                        "message": "Expected Equal `=` but found LeftBrace `{`"
                    }
                ]
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
                            "character": 19
                        },
                        "end": {
                            "line": 0,
                            "character": 20
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Expected Equal `=` but found LeftBrace `{`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 0,
                                        "character": 19
                                    },
                                    "end": {
                                        "line": 0,
                                        "character": 20
                                    }
                                }
                            },
                            "message": "note: A function type annotation cannot include a function body."
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 0,
                                        "character": 19
                                    },
                                    "end": {
                                        "line": 0,
                                        "character": 20
                                    }
                                }
                            },
                            "message": "help: Function definitions use `::`; did you mean to write `::` instead of `:`?"
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
                "title": "Change `:` to `::`",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "file:///test.n": [
                            {
                                "range": {
                                    "start": {
                                        "line": 0,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 0,
                                        "character": 5
                                    }
                                },
                                "newText": "::"
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
