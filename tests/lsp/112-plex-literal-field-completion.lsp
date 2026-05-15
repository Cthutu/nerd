Point :: plex {
    x i32
    y i32
    visible bool
}

main :: fn () {
    point := Point {
        x: 1
        v
        q
    }
}
¬
[
    {
        "jsonrpc": "2.0",
        "method": "textDocument/didOpen",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/112-plex-literal-field-completion/main.n",
                "languageId": "nerd",
                "version": 1,
                "text": "win :: use win\n\nPoint :: plex {\n    x i32\n    y i32\n    visible bool\n}\n\nmain :: fn () {\n    point := Point {\n        x: 1\n        v\n        q\n    }\n    class := win.WNDCLASSA {\n        style: 0\n        lp\n    }\n}\n"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/completion",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/112-plex-literal-field-completion/main.n"
            },
            "position": {
                "line": 11,
                "character": 9
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "method": "textDocument/completion",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/112-plex-literal-field-completion/main.n"
            },
            "position": {
                "line": 16,
                "character": 10
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "method": "textDocument/completion",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/112-plex-literal-field-completion/main.n"
            },
            "position": {
                "line": 12,
                "character": 9
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
            "diagnostics": [
                {
                    "range": {
                        "start": {
                            "line": 9,
                            "character": 8
                        },
                        "end": {
                            "line": 9,
                            "character": 9
                        }
                    },
                    "severity": 1,
                    "code": "0304",
                    "source": "nerd",
                    "message": "Type mismatch: expected `known plex field`, found `v`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 9,
                                        "character": 8
                                    },
                                    "end": {
                                        "line": 9,
                                        "character": 9
                                    }
                                }
                            },
                            "message": "help: Change the expression or annotation so both sides use the same type."
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
            "uri": "__REPO_URI__/tests/lsp/112-plex-literal-field-completion/main.n",
            "diagnostics": [
                {
                    "range": {
                        "start": {
                            "line": 11,
                            "character": 8
                        },
                        "end": {
                            "line": 11,
                            "character": 9
                        }
                    },
                    "severity": 1,
                    "code": "0304",
                    "source": "nerd",
                    "message": "Type mismatch: expected `known plex field`, found `v`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "__REPO_URI__/tests/lsp/112-plex-literal-field-completion/main.n",
                                "range": {
                                    "start": {
                                        "line": 11,
                                        "character": 8
                                    },
                                    "end": {
                                        "line": 11,
                                        "character": 9
                                    }
                                }
                            },
                            "message": "help: Change the expression or annotation so both sides use the same type."
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
                "label": "visible",
                "kind": 5,
                "insertText": "visible: "
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": [
            {
                "label": "lpfnWndProc",
                "kind": 5,
                "insertText": "lpfnWndProc: "
            },
            {
                "label": "lpszClassName",
                "kind": 5,
                "insertText": "lpszClassName: "
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": []
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
