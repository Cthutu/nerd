-- lsp-uri: __REPO_URI__/tests/lsp/130-workspace-root-add-use/tiny_engine/graphics.n
-- lsp-root-uri: __REPO_URI__/tests/lsp/130-workspace-root-add-use
Graphics :: plex {
    wnd HWND
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/codeAction",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/130-workspace-root-add-use/tiny_engine/graphics.n"
            },
            "range": {
                "start": {
                    "line": 1,
                    "character": 8
                },
                "end": {
                    "line": 1,
                    "character": 12
                }
            },
            "context": {
                "diagnostics": []
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
            "uri": "__REPO_URI__/tests/lsp/130-workspace-root-add-use/tiny_engine/graphics.n",
            "diagnostics": [
                {
                    "range": {
                        "start": {
                            "line": 1,
                            "character": 8
                        },
                        "end": {
                            "line": 1,
                            "character": 12
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Unknown type `HWND`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "__REPO_URI__/tests/lsp/130-workspace-root-add-use/tiny_engine/graphics.n",
                                "range": {
                                    "start": {
                                        "line": 1,
                                        "character": 8
                                    },
                                    "end": {
                                        "line": 1,
                                        "character": 12
                                    }
                                }
                            },
                            "message": "help: Use a defined type name, or one of the built-in primitive types."
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
                "title": "Add use windows.user",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "__REPO_URI__/tests/lsp/130-workspace-root-add-use/tiny_engine/graphics.n": [
                            {
                                "range": {
                                    "start": {
                                        "line": 0,
                                        "character": 0
                                    },
                                    "end": {
                                        "line": 0,
                                        "character": 0
                                    }
                                },
                                "newText": "use windows.user\n\n"
                            }
                        ]
                    }
                }
            },
            {
                "title": "Add use os.windows.user",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "__REPO_URI__/tests/lsp/130-workspace-root-add-use/tiny_engine/graphics.n": [
                            {
                                "range": {
                                    "start": {
                                        "line": 0,
                                        "character": 0
                                    },
                                    "end": {
                                        "line": 0,
                                        "character": 0
                                    }
                                },
                                "newText": "use os.windows.user\n\n"
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
