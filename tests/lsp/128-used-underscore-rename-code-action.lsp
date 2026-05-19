main :: fn () -> i32 {
    _result := 1
    return _result
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
                    "line": 2,
                    "character": 11
                },
                "end": {
                    "line": 2,
                    "character": 18
                }
            },
            "context": {
                "diagnostics": [
                    {
                        "range": {
                            "start": {
                                "line": 2,
                                "character": 11
                            },
                            "end": {
                                "line": 2,
                                "character": 18
                            }
                        },
                        "severity": 1,
                        "source": "nerd",
                        "message": "Used local variable `_result` marked as unused"
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
                            "line": 2,
                            "character": 11
                        },
                        "end": {
                            "line": 2,
                            "character": 18
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Used local variable `_result` marked as unused",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 1,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 1,
                                        "character": 11
                                    }
                                }
                            },
                            "message": "`_result` is marked unused by its leading `_`"
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 2,
                                        "character": 11
                                    },
                                    "end": {
                                        "line": 2,
                                        "character": 18
                                    }
                                }
                            },
                            "message": "note: Leading `_` names are reserved for bindings that are deliberately unused."
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 2,
                                        "character": 11
                                    },
                                    "end": {
                                        "line": 2,
                                        "character": 18
                                    }
                                }
                            },
                            "message": "help: Rename `_result` without the leading `_` now that it is used."
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
                "title": "Rename to result",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "file:///test.n": [
                            {
                                "range": {
                                    "start": {
                                        "line": 1,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 1,
                                        "character": 11
                                    }
                                },
                                "newText": "result"
                            },
                            {
                                "range": {
                                    "start": {
                                        "line": 2,
                                        "character": 11
                                    },
                                    "end": {
                                        "line": 2,
                                        "character": 18
                                    }
                                },
                                "newText": "result"
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
