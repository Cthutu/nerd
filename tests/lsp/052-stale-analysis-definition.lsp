State :: plex { inventory i32 }

main :: fn () {
    game :: State { inventory: 1 }
    game.inventory
}
¬
[
    {
        "jsonrpc": "2.0",
        "method": "textDocument/didChange",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "contentChanges": [
                {
                    "text": "State :: plex { inventory i32 }\n\nmain :: fn () {\n    game :: State { inventory: 1 }\n    game.inventory\n    broken :\n}\n"
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/definition",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 4,
                "character": 9
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
            "diagnostics": []
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
                            "line": 6,
                            "character": 0
                        },
                        "end": {
                            "line": 6,
                            "character": 1
                        }
                    },
                    "severity": 1,
                    "code": "0205",
                    "source": "nerd",
                    "message": "Expected declaration or expression but found RightBrace `}`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 6,
                                        "character": 0
                                    },
                                    "end": {
                                        "line": 6,
                                        "character": 1
                                    }
                                }
                            },
                            "message": "help: Expected a type annotation after ':', but found RightBrace `}`"
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
            "uri": "file:///test.n",
            "range": {
                "start": {
                    "line": 0,
                    "character": 16
                },
                "end": {
                    "line": 0,
                    "character": 25
                }
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
