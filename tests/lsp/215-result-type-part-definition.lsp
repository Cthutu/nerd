-- lsp-uri: __REPO_URI__/tests/lsp/215-result-type-part-definition/api.n
pub fetch :: fn () -> Value\Failure { return Value { number: 1 } }
pub other :: fn () -> Value\OtherFailure { return Value { number: 2 } }
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/definition",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/215-result-type-part-definition/api.n"
            },
            "position": {
                "line": 0,
                "character": 23
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "method": "textDocument/definition",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/215-result-type-part-definition/api.n"
            },
            "position": {
                "line": 0,
                "character": 29
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "method": "textDocument/definition",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/215-result-type-part-definition/api.n"
            },
            "position": {
                "line": 0,
                "character": 5
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "method": "textDocument/definition",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/215-result-type-part-definition/api.n"
            },
            "position": {
                "line": 1,
                "character": 29
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
            "uri": "__REPO_URI__/tests/lsp/215-result-type-part-definition/api.n",
            "diagnostics": []
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 2,
        "result": {
            "uri": "__REPO_URI__/tests/lsp/215-result-type-part-definition/mod.n",
            "range": {
                "start": {
                    "line": 0,
                    "character": 4
                },
                "end": {
                    "line": 0,
                    "character": 9
                }
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "uri": "__REPO_URI__/tests/lsp/215-result-type-part-definition/mod.n",
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
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": {
            "uri": "__REPO_URI__/tests/lsp/215-result-type-part-definition/api.n",
            "range": {
                "start": {
                    "line": 0,
                    "character": 4
                },
                "end": {
                    "line": 0,
                    "character": 9
                }
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "result": {
            "uri": "__REPO_URI__/tests/lsp/215-result-type-part-definition/types.n",
            "range": {
                "start": {
                    "line": 0,
                    "character": 4
                },
                "end": {
                    "line": 0,
                    "character": 16
                }
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
