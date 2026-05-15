main :: fn () {}
¬
[
    {
        "jsonrpc": "2.0",
        "method": "textDocument/didOpen",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/086-rename-with-import-error/main.n",
                "languageId": "nerd",
                "version": 1,
                "text": "bad :: use bad\n\nmain :: fn () -> i32 {\n    _value := 41\n    return _value\n}\n"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/prepareRename",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/086-rename-with-import-error/main.n"
            },
            "position": {
                "line": 3,
                "character": 5
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "method": "textDocument/rename",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/086-rename-with-import-error/main.n"
            },
            "position": {
                "line": 4,
                "character": 12
            },
            "newName": "value"
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
        "method": "textDocument/publishDiagnostics",
        "params": {
            "uri": "__REPO_URI__/tests/lsp/086-rename-with-import-error/main.n",
            "diagnostics": []
        }
    },
    {
        "jsonrpc": "2.0",
        "method": "textDocument/publishDiagnostics",
        "params": {
            "uri": "__REPO_URI__/tests/lsp/086-rename-with-import-error/bad/mod.n",
            "diagnostics": [
                {
                    "range": {
                        "start": {
                            "line": 0,
                            "character": 13
                        },
                        "end": {
                            "line": 0,
                            "character": 20
                        }
                    },
                    "severity": 1,
                    "code": "0300",
                    "source": "nerd",
                    "message": "Unknown symbol `missing`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "__REPO_URI__/tests/lsp/086-rename-with-import-error/bad/mod.n",
                                "range": {
                                    "start": {
                                        "line": 0,
                                        "character": 13
                                    },
                                    "end": {
                                        "line": 0,
                                        "character": 20
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
            "range": {
                "start": {
                    "line": 3,
                    "character": 4
                },
                "end": {
                    "line": 3,
                    "character": 10
                }
            },
            "placeholder": "_value"
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "changes": {
                "__REPO_URI__/tests/lsp/086-rename-with-import-error/main.n": [
                    {
                        "range": {
                            "start": {
                                "line": 3,
                                "character": 4
                            },
                            "end": {
                                "line": 3,
                                "character": 10
                            }
                        },
                        "newText": "value"
                    },
                    {
                        "range": {
                            "start": {
                                "line": 4,
                                "character": 11
                            },
                            "end": {
                                "line": 4,
                                "character": 17
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
        "id": 999,
        "result": null
    }
]
