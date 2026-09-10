Render :: trait { render :: fn (self: ^Self) }
Thing :: plex { value i32 }
impl Render for Thing { render :: fn (self: ^Self) {} }
main :: fn () {}
¬
[
    {
        "jsonrpc": "2.0",
        "method": "textDocument/didChange",
        "params": {
            "textDocument": {
                "uri": "file:///test.n",
                "version": 2
            },
            "contentChanges": [
                {
                    "text": "Render :: trait { render :: fn (self: ^Self) }\nThing :: plex { value i32 }\nimpl Render for Thing { render :: fn (_self: ^Self) {} }\nmain :: fn () {}\n"
                }
            ]
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
                            "line": 2,
                            "character": 38
                        },
                        "end": {
                            "line": 2,
                            "character": 42
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Unused parameter `self`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 2,
                                        "character": 38
                                    },
                                    "end": {
                                        "line": 2,
                                        "character": 42
                                    }
                                }
                            },
                            "message": "note: Assigning to a variable does not count as using it."
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 2,
                                        "character": 38
                                    },
                                    "end": {
                                        "line": 2,
                                        "character": 42
                                    }
                                }
                            },
                            "message": "help: Remove `self` or prefix the name with `_` if it is deliberately unused."
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
            "uri": "file:///test.n",
            "diagnostics": []
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
