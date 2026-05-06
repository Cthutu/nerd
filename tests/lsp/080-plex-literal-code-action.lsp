Point :: plex {
    x i32
    y i32
    visible bool
    name string
}

main :: fn () {
    point := Point {
        x: 1
    }
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/codeAction",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "range": {
                "start": {
                    "line": 8,
                    "character": 20
                },
                "end": {
                    "line": 8,
                    "character": 20
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
                            "line": 8,
                            "character": 19
                        },
                        "end": {
                            "line": 8,
                            "character": 20
                        }
                    },
                    "severity": 1,
                    "code": "0304",
                    "source": "nerd",
                    "message": "Type mismatch: expected `all plex fields`, found `different field count`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 8,
                                        "character": 19
                                    },
                                    "end": {
                                        "line": 8,
                                        "character": 20
                                    }
                                }
                            },
                            "message": "note: Missing fields: `y`, `visible`, `name`"
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 8,
                                        "character": 19
                                    },
                                    "end": {
                                        "line": 8,
                                        "character": 20
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
                "title": "Fill missing plex fields",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "file:///test.n": [
                            {
                                "range": {
                                    "start": {
                                        "line": 10,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 10,
                                        "character": 4
                                    }
                                },
                                "newText": "\n        y: 0\n        visible: no\n        name: \"\"\n    "
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
