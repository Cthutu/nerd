main :: fn () {}
¬
[
    {
        "jsonrpc": "2.0",
        "method": "textDocument/didOpen",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/078-current-module-completion/main.n",
                "languageId": "nerd",
                "version": 1,
                "text": "x11 :: use x11\n\nmain :: fn () {\n        \n}\n"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "method": "textDocument/didChange",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/078-current-module-completion/main.n",
                "version": 2
            },
            "contentChanges": [
                {
                    "range": {
                        "start": {
                            "line": 3,
                            "character": 8
                        },
                        "end": {
                            "line": 3,
                            "character": 8
                        }
                    },
                    "text": "display := x11"
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "method": "textDocument/didChange",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/078-current-module-completion/main.n",
                "version": 3
            },
            "contentChanges": [
                {
                    "range": {
                        "start": {
                            "line": 3,
                            "character": 22
                        },
                        "end": {
                            "line": 3,
                            "character": 22
                        }
                    },
                    "text": "."
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/completion",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/078-current-module-completion/main.n"
            },
            "position": {
                "line": 3,
                "character": 23
            },
            "context": {
                "triggerKind": 2,
                "triggerCharacter": "."
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
            "uri": "__REPO_URI__/tests/lsp/078-current-module-completion/main.n",
            "diagnostics": []
        }
    },
    {
        "jsonrpc": "2.0",
        "method": "textDocument/publishDiagnostics",
        "params": {
            "uri": "__REPO_URI__/tests/lsp/078-current-module-completion/main.n",
            "diagnostics": [
                {
                    "range": {
                        "start": {
                            "line": 3,
                            "character": 8
                        },
                        "end": {
                            "line": 3,
                            "character": 15
                        }
                    },
                    "severity": 1,
                    "code": "0306",
                    "source": "nerd",
                    "message": "Invalid variable type `module`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "__REPO_URI__/tests/lsp/078-current-module-completion/main.n",
                                "range": {
                                    "start": {
                                        "line": 3,
                                        "character": 8
                                    },
                                    "end": {
                                        "line": 3,
                                        "character": 15
                                    }
                                }
                            },
                            "message": "help: Variables may use concrete integer, `bool`, `string`, `f32`, or `f64` types."
                        }
                    ]
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "method": "textDocument/publishDiagnostics",
        "params": {
            "uri": "__REPO_URI__/tests/lsp/078-current-module-completion/main.n",
            "diagnostics": [
                {
                    "range": {
                        "start": {
                            "line": 4,
                            "character": 0
                        },
                        "end": {
                            "line": 4,
                            "character": 1
                        }
                    },
                    "severity": 1,
                    "code": "0203",
                    "source": "nerd",
                    "message": "Expected Symbol but found RightBrace `}`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "__REPO_URI__/tests/lsp/078-current-module-completion/main.n",
                                "range": {
                                    "start": {
                                        "line": 4,
                                        "character": 0
                                    },
                                    "end": {
                                        "line": 4,
                                        "character": 1
                                    }
                                }
                            },
                            "message": "help: Check for a missing closing delimiter or misplaced operator"
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
                "label": "Display",
                "kind": 22
            },
            {
                "label": "Window",
                "kind": 22
            },
            {
                "label": "XOpenDisplay",
                "kind": 3
            },
            {
                "label": "XCloseDisplay",
                "kind": 3
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
