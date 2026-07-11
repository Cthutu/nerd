use core

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
    VAO.new().
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
                "line": 16,
                "character": 14
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
                            "line": 17,
                            "character": 0
                        },
                        "end": {
                            "line": 17,
                            "character": 1
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Expected Symbol but found RightBrace `}`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 17,
                                        "character": 0
                                    },
                                    "end": {
                                        "line": 17,
                                        "character": 1
                                    }
                                }
                            },
                            "message": "help: Check for a missing closing delimiter or misplaced operator"
                        }
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
