-- Core bindings are implicit.

VAO :: plex {
    id i32
}

impl VAO {
    new :: fn () -> ?Self {
        return Self { id: 1 }
    }

    bind :: fn (self: ^Self) {
    }
}

main :: fn () {
    on VAO.new() => [vao] {
        vao.bind()
        vao.missing()
    }
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
                "line": 17,
                "character": 12
            },
            "context": {
                "triggerKind": 2,
                "triggerCharacter": "."
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
                            "line": 18,
                            "character": 12
                        },
                        "end": {
                            "line": 18,
                            "character": 19
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Unknown member `missing` for `VAO`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 18,
                                        "character": 12
                                    },
                                    "end": {
                                        "line": 18,
                                        "character": 19
                                    }
                                }
                            },
                            "message": "help: Use a field or method that exists on `VAO`."
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
                "label": "id",
                "kind": 5,
                "detail": "field"
            },
            {
                "label": "bind",
                "kind": 2,
                "detail": "method"
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
