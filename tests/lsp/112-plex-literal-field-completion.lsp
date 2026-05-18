use win

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
    class := WNDCLASSA {
        style: 0
        l
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
                "text": "win :: use win\nuse win\n\nPoint :: plex {\n    x i32\n    y i32\n    visible bool\n}\n\nmain :: fn () {\n    point := Point {\n        x: 1\n        v\n        q\n    }\n    class := win.WNDCLASSA {\n        style: 0\n        lp\n    }\n    unqualified := WNDCLASSA {\n        style: 0\n        l\n    }\n}\n"
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
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "method": "textDocument/completion",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/112-plex-literal-field-completion/main.n"
            },
            "position": {
                "line": 20,
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
                            "character": 4
                        },
                        "end": {
                            "line": 0,
                            "character": 7
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Type mismatch: expected `known module`, found `module path`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 0,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 0,
                                        "character": 7
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
                            "line": 12,
                            "character": 8
                        },
                        "end": {
                            "line": 12,
                            "character": 9
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Type mismatch: expected `known plex field`, found `v`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "__REPO_URI__/tests/lsp/112-plex-literal-field-completion/main.n",
                                "range": {
                                    "start": {
                                        "line": 12,
                                        "character": 8
                                    },
                                    "end": {
                                        "line": 12,
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
        "result": []
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": []
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
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
        "id": 5,
        "result": []
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
