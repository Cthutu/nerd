answer :: 41
counter : i32 = 1

main :: fn () -> i32 {
    counter = counter + answer
    answer
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/rename",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 0,
                "character": 2
            },
            "newName": "value"
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "method": "textDocument/rename",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 1,
                "character": 2
            },
            "newName": "total"
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
                            "line": 5,
                            "character": 4
                        },
                        "end": {
                            "line": 5,
                            "character": 10
                        }
                    },
                    "severity": 1,
                    "code": "0345",
                    "source": "nerd",
                    "message": "Expression result of type `i32` is not used",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 5,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 5,
                                        "character": 10
                                    }
                                }
                            },
                            "message": "note: Only `void` expressions can be used as standalone statements."
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 5,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 5,
                                        "character": 10
                                    }
                                }
                            },
                            "message": "help: Bind the result to `_` when the value is intentionally ignored."
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
            "changes": {
                "file:///test.n": [
                    {
                        "range": {
                            "start": {
                                "line": 0,
                                "character": 0
                            },
                            "end": {
                                "line": 0,
                                "character": 6
                            }
                        },
                        "newText": "value"
                    },
                    {
                        "range": {
                            "start": {
                                "line": 4,
                                "character": 24
                            },
                            "end": {
                                "line": 4,
                                "character": 30
                            }
                        },
                        "newText": "value"
                    },
                    {
                        "range": {
                            "start": {
                                "line": 5,
                                "character": 4
                            },
                            "end": {
                                "line": 5,
                                "character": 10
                            }
                        },
                        "newText": "value"
                    }
                ]
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "changes": {
                "file:///test.n": [
                    {
                        "range": {
                            "start": {
                                "line": 1,
                                "character": 0
                            },
                            "end": {
                                "line": 1,
                                "character": 7
                            }
                        },
                        "newText": "total"
                    },
                    {
                        "range": {
                            "start": {
                                "line": 4,
                                "character": 4
                            },
                            "end": {
                                "line": 4,
                                "character": 11
                            }
                        },
                        "newText": "total"
                    },
                    {
                        "range": {
                            "start": {
                                "line": 4,
                                "character": 14
                            },
                            "end": {
                                "line": 4,
                                "character": 21
                            }
                        },
                        "newText": "total"
                    }
                ]
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
