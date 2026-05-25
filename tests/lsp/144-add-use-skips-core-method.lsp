-- lsp-uri: __REPO_URI__/tests/lsp/144-add-use-skips-core-method.input.n
main :: fn () {
    alloc()
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/codeAction",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/144-add-use-skips-core-method.input.n"
            },
            "range": {
                "start": {
                    "line": 1,
                    "character": 5
                },
                "end": {
                    "line": 1,
                    "character": 5
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
            "uri": "__REPO_URI__/tests/lsp/144-add-use-skips-core-method.input.n",
            "diagnostics": [
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
                    "severity": 1,
                    "source": "nerd",
                    "message": "Unknown symbol `alloc`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "__REPO_URI__/tests/lsp/144-add-use-skips-core-method.input.n",
                                "range": {
                                    "start": {
                                        "line": 1,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 1,
                                        "character": 9
                                    }
                                }
                            },
                            "message": "help: Add a binding for `alloc` or fix the spelling."
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
                "title": "Add use std.mem",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "__REPO_URI__/tests/lsp/144-add-use-skips-core-method.input.n": [
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
                                "newText": "use std.mem\n\n"
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
