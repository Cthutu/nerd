-- lsp-uri: __REPO_URI__/tests/lsp/165-add-use-local-type-code-action/hello.n
main :: fn () {
    value : VAO
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/codeAction",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/lsp/165-add-use-local-type-code-action/hello.n"
            },
            "range": {
                "start": {
                    "line": 1,
                    "character": 12
                },
                "end": {
                    "line": 1,
                    "character": 15
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
            "uri": "__REPO_URI__/tests/lsp/165-add-use-local-type-code-action/hello.n",
            "diagnostics": [
                {
                    "range": {
                        "start": {
                            "line": 1,
                            "character": 12
                        },
                        "end": {
                            "line": 1,
                            "character": 15
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Unknown type `VAO`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "__REPO_URI__/tests/lsp/165-add-use-local-type-code-action/hello.n",
                                "range": {
                                    "start": {
                                        "line": 1,
                                        "character": 12
                                    },
                                    "end": {
                                        "line": 1,
                                        "character": 15
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
                "title": "Add use vao",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "__REPO_URI__/tests/lsp/165-add-use-local-type-code-action/hello.n": [
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
                                "newText": "use vao\n\n"
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
