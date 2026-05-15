use win

main :: fn () {
    result := DefWindowProcA(
}
¬
[
    {
        "jsonrpc": "2.0",
        "method": "textDocument/didOpen",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/111-imported-ffi-signature-help/main.n",
                "languageId": "nerd",
                "version": 1,
                "text": "use win\n\nmain :: fn () {\n    result := DefWindowProcA(\n}\n"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/signatureHelp",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/111-imported-ffi-signature-help/main.n"
            },
            "position": {
                "line": 3,
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
                    "code": "0201",
                    "source": "nerd",
                    "message": "Missing value before RightBrace `}`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
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
                            "message": "help: Insert a literal, parenthesized expression, or unary operator"
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
            "uri": "file:///C%3A/Users/matt/nerd/tests/lsp/111-imported-ffi-signature-help/main.n",
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
                    "code": "0201",
                    "source": "nerd",
                    "message": "Missing value before RightBrace `}`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///C%3A/Users/matt/nerd/tests/lsp/111-imported-ffi-signature-help/main.n",
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
                            "message": "help: Insert a literal, parenthesized expression, or unary operator"
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
            "signatures": [
                {
                    "label": "DefWindowProcA(hWnd: HWND, Msg: UINT, wParam: WPARAM, lParam: LPARAM) -> LRESULT",
                    "documentation": "Named arguments use `name = value`; omitted parameters use declared defaults when available.",
                    "parameters": [
                        {
                            "label": "hWnd: HWND"
                        },
                        {
                            "label": "Msg: UINT"
                        },
                        {
                            "label": "wParam: WPARAM"
                        },
                        {
                            "label": "lParam: LPARAM"
                        }
                    ]
                }
            ],
            "activeSignature": 0,
            "activeParameter": 0
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
