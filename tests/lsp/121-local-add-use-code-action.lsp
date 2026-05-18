-- lsp-uri: __REPO_URI__/tests/lsp/121-local-add-use-code-action.input.n
main :: fn () {
    local_helper()
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/codeAction",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/121-local-add-use-code-action.input.n"
            },
            "range": {
                "start": {
                    "line": 1,
                    "character": 8
                },
                "end": {
                    "line": 1,
                    "character": 8
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
            "uri": "__REPO_URI__/tests/lsp/121-local-add-use-code-action.input.n",
            "diagnostics": [
                {
                    "range": {
                        "start": {
                            "line": 1,
                            "character": 4
                        },
                        "end": {
                            "line": 1,
                            "character": 16
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Unknown symbol `local_helper`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "__REPO_URI__/tests/lsp/121-local-add-use-code-action.input.n",
                                "range": {
                                    "start": {
                                        "line": 1,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 1,
                                        "character": 16
                                    }
                                }
                            },
                            "message": "help: Add a binding for `local_helper` or fix the spelling."
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
                "title": "Add use local_import_fixture",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "__REPO_URI__/tests/lsp/121-local-add-use-code-action.input.n": [
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
                                "newText": "use local_import_fixture\n\n"
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
