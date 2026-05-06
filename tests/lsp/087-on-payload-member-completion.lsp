KeyEvent :: plex {
    code i32
    mods i32
}

Event :: enum {
    KeyDown(KeyEvent)
}

main :: fn () {
    event : Event
    on event {
        KeyDown(as key_event) => {
            key_event.
        }
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
                "line": 13,
                "character": 22
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
                        "."
                    ],
                    "resolveProvider": false
                },
                "signatureHelpProvider": {
                    "triggerCharacters": [
                        "(",
                        ","
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
                            "line": 14,
                            "character": 8
                        },
                        "end": {
                            "line": 14,
                            "character": 9
                        }
                    },
                    "severity": 1,
                    "code": "0203",
                    "source": "nerd",
                    "message": "Expected Symbol but found RightBrace `}`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 14,
                                        "character": 8
                                    },
                                    "end": {
                                        "line": 14,
                                        "character": 9
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
        "result": [
            {
                "label": "code",
                "kind": 5
            },
            {
                "label": "mods",
                "kind": 5
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
