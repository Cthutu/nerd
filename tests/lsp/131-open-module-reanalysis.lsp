-- lsp-uri: __REPO_URI__/tests/lsp/131-open-module-reanalysis/bar.n
-- lsp-root-uri: __REPO_URI__/tests/lsp/131-open-module-reanalysis
use foo

value :: fn (_arg: Foo) {}
¬
[
    {
        "jsonrpc": "2.0",
        "method": "textDocument/didOpen",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/131-open-module-reanalysis/foo.n",
                "languageId": "nerd",
                "version": 1,
                "text": "pub Foo :: usize\n"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/codeAction",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/131-open-module-reanalysis/bar.n"
            },
            "range": {
                "start": {
                    "line": 2,
                    "character": 19
                },
                "end": {
                    "line": 2,
                    "character": 22
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
            "uri": "__REPO_URI__/tests/lsp/131-open-module-reanalysis/bar.n",
            "diagnostics": [
                {
                    "range": {
                        "start": {
                            "line": 2,
                            "character": 19
                        },
                        "end": {
                            "line": 2,
                            "character": 22
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Unknown type `Foo`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "__REPO_URI__/tests/lsp/131-open-module-reanalysis/bar.n",
                                "range": {
                                    "start": {
                                        "line": 2,
                                        "character": 19
                                    },
                                    "end": {
                                        "line": 2,
                                        "character": 22
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
        "method": "textDocument/publishDiagnostics",
        "params": {
            "uri": "__REPO_URI__/tests/lsp/131-open-module-reanalysis/foo.n",
            "diagnostics": []
        }
    },
    {
        "jsonrpc": "2.0",
        "method": "textDocument/publishDiagnostics",
        "params": {
            "uri": "__REPO_URI__/tests/lsp/131-open-module-reanalysis/bar.n",
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
                    "severity": 4,
                    "source": "nerd",
                    "message": "Unused use `foo`",
                    "tags": [
                        1
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
        "id": 999,
        "result": null
    }
]
