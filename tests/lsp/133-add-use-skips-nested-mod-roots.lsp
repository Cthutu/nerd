-- lsp-uri: __REPO_URI__/examples/text-adventure/quill.n
-- lsp-root-uri: __REPO_URI__
main :: fn () {
    parts := "a b".split(" ")
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/codeAction",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/examples/text-adventure/quill.n"
            },
            "range": {
                "start": {
                    "line": 1,
                    "character": 19
                },
                "end": {
                    "line": 1,
                    "character": 19
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
            "uri": "__REPO_URI__/examples/text-adventure/quill.n",
            "diagnostics": [
                {
                    "range": {
                        "start": {
                            "line": 1,
                            "character": 19
                        },
                        "end": {
                            "line": 1,
                            "character": 24
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Type mismatch: expected `string field `.data`, `.count`, or `.bytes``, found `split`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "__REPO_URI__/examples/text-adventure/quill.n",
                                "range": {
                                    "start": {
                                        "line": 1,
                                        "character": 19
                                    },
                                    "end": {
                                        "line": 1,
                                        "character": 24
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
                "title": "Add use std.string",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "__REPO_URI__/examples/text-adventure/quill.n": [
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
                                "newText": "use std.string\n\n"
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
