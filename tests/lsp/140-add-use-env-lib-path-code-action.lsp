-- lsp-uri: file:///tmp/nerd-env-code-action/main.n
-- lsp-root-uri: file:///tmp/nerd-env-code-action
-- lsp-lib-path: __REPO_URI__/tests/lsp/env_lib
main :: fn () {
    env_only_helper()
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/codeAction",
        "params": {
            "textDocument": {
                "uri": "file:///tmp/nerd-env-code-action/main.n"
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
            "uri": "file:///tmp/nerd-env-code-action/main.n",
            "diagnostics": [
                {
                    "range": {
                        "start": {
                            "line": 1,
                            "character": 4
                        },
                        "end": {
                            "line": 1,
                            "character": 19
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Unknown symbol `env_only_helper`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///tmp/nerd-env-code-action/main.n",
                                "range": {
                                    "start": {
                                        "line": 1,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 1,
                                        "character": 19
                                    }
                                }
                            },
                            "message": "help: Add a binding for `env_only_helper` or fix the spelling."
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
                "title": "Add use envonly",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "file:///tmp/nerd-env-code-action/main.n": [
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
                                "newText": "use envonly\n\n"
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
