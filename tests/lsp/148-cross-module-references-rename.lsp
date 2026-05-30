-- lsp-uri: __REPO_URI__/tests/lsp/148-cross-module-references-rename/main.n
-- lsp-root-uri: __REPO_URI__/tests/lsp/148-cross-module-references-rename
use helper

main :: fn () -> i32 {
    return answer + twice()
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/references",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/148-cross-module-references-rename/main.n"
            },
            "position": {
                "line": 3,
                "character": 11
            },
            "context": {
                "includeDeclaration": true
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "method": "textDocument/references",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/148-cross-module-references-rename/main.n"
            },
            "position": {
                "line": 3,
                "character": 11
            },
            "context": {
                "includeDeclaration": false
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "method": "textDocument/rename",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/148-cross-module-references-rename/main.n"
            },
            "position": {
                "line": 3,
                "character": 11
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
            "uri": "__REPO_URI__/tests/lsp/148-cross-module-references-rename/main.n",
            "diagnostics": []
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 2,
        "result": [
            {
                "uri": "__REPO_URI__/tests/lsp/148-cross-module-references-rename/main.n",
                "range": {
                    "start": {
                        "line": 3,
                        "character": 11
                    },
                    "end": {
                        "line": 3,
                        "character": 17
                    }
                }
            },
            {
                "uri": "__REPO_URI__/tests/lsp/148-cross-module-references-rename/helper.n",
                "range": {
                    "start": {
                        "line": 0,
                        "character": 4
                    },
                    "end": {
                        "line": 0,
                        "character": 10
                    }
                }
            },
            {
                "uri": "__REPO_URI__/tests/lsp/148-cross-module-references-rename/helper.n",
                "range": {
                    "start": {
                        "line": 3,
                        "character": 11
                    },
                    "end": {
                        "line": 3,
                        "character": 17
                    }
                }
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": [
            {
                "uri": "__REPO_URI__/tests/lsp/148-cross-module-references-rename/main.n",
                "range": {
                    "start": {
                        "line": 3,
                        "character": 11
                    },
                    "end": {
                        "line": 3,
                        "character": 17
                    }
                }
            },
            {
                "uri": "__REPO_URI__/tests/lsp/148-cross-module-references-rename/helper.n",
                "range": {
                    "start": {
                        "line": 3,
                        "character": 11
                    },
                    "end": {
                        "line": 3,
                        "character": 17
                    }
                }
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": {
            "changes": {
                "__REPO_URI__/tests/lsp/148-cross-module-references-rename/main.n": [
                    {
                        "range": {
                            "start": {
                                "line": 3,
                                "character": 11
                            },
                            "end": {
                                "line": 3,
                                "character": 17
                            }
                        },
                        "newText": "count"
                    }
                ],
                "__REPO_URI__/tests/lsp/148-cross-module-references-rename/helper.n": [
                    {
                        "range": {
                            "start": {
                                "line": 0,
                                "character": 4
                            },
                            "end": {
                                "line": 0,
                                "character": 10
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
                                "character": 17
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
