Thing :: plex {
    value i32
}

broken : fn(frame: ^Thing) {
    fra
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/completion",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 5,
                "character": 7
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
                            "character": 9
                        },
                        "end": {
                            "line": 4,
                            "character": 11
                        }
                    },
                    "severity": 1,
                    "code": "0203",
                    "source": "nerd",
                    "message": "Expected Colon `:` but found Keyword `fn`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 4,
                                        "character": 9
                                    },
                                    "end": {
                                        "line": 4,
                                        "character": 11
                                    }
                                }
                            },
                            "message": "note: Function values use a binding operator before `fn`, not a type annotation colon."
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 4,
                                        "character": 9
                                    },
                                    "end": {
                                        "line": 4,
                                        "character": 11
                                    }
                                }
                            },
                            "message": "help: Use `:: fn (...) { ... }` for a constant function binding, or `:= fn (...) { ... }` for a mutable function value."
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
                "label": "frame",
                "kind": 6
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
