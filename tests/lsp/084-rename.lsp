main :: fn () -> i32 {
    value := 1
    value = value + 1
    return value
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/prepareRename",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 2,
                "character": 12
            }
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
                "line": 2,
                "character": 12
            },
            "newName": "count"
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
                        "."
                    ],
                    "resolveProvider": false
                },
                "signatureHelpProvider": {
                    "triggerCharacters": [
                        "(",
                        ","
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
            "diagnostics": []
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 2,
        "result": {
            "range": {
                "start": {
                    "line": 2,
                    "character": 12
                },
                "end": {
                    "line": 2,
                    "character": 17
                }
            },
            "placeholder": "value"
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
                                "character": 4
                            },
                            "end": {
                                "line": 1,
                                "character": 9
                            }
                        },
                        "newText": "count"
                    },
                    {
                        "range": {
                            "start": {
                                "line": 2,
                                "character": 4
                            },
                            "end": {
                                "line": 2,
                                "character": 9
                            }
                        },
                        "newText": "count"
                    },
                    {
                        "range": {
                            "start": {
                                "line": 2,
                                "character": 12
                            },
                            "end": {
                                "line": 2,
                                "character": 17
                            }
                        },
                        "newText": "count"
                    },
                    {
                        "range": {
                            "start": {
                                "line": 3,
                                "character": 11
                            },
                            "end": {
                                "line": 3,
                                "character": 16
                            }
                        },
                        "newText": "count"
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
