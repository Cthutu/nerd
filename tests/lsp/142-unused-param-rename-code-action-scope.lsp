main :: fn () {
    map := 1
    use_map(map)
}

simulate :: fn () {
    map := 2
    use_map(map)
}

use_map :: fn (value: i32) {
    on value < 0 => return
}

generate_map :: fn (map: i32) {
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
                    "line": 14,
                    "character": 20
                },
                "end": {
                    "line": 14,
                    "character": 23
                }
            },
            "context": {
                "diagnostics": [
                    {
                        "range": {
                            "start": {
                                "line": 14,
                                "character": 20
                            },
                            "end": {
                                "line": 14,
                                "character": 23
                            }
                        },
                        "severity": 1,
                        "source": "nerd",
                        "message": "Unused parameter `map`"
                    }
                ]
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
                        "line": 14,
                        "character": 20
                    },
                    "end": {
                        "line": 14,
                        "character": 23
                    }
                },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Unused parameter `map`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 14,
                                        "character": 20
                                    },
                                    "end": {
                                        "line": 14,
                                        "character": 23
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
                                        "line": 14,
                                        "character": 20
                                    },
                                    "end": {
                                        "line": 14,
                                        "character": 23
                                    }
                                }
                            },
                            "message": "help: Remove `map` or prefix the name with `_` if it is deliberately unused."
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
                "title": "Rename to _map",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "file:///test.n": [
                            {
                                "range": {
                                    "start": {
                                        "line": 14,
                                        "character": 20
                                    },
                                    "end": {
                                        "line": 14,
                                        "character": 23
                                    }
                                },
                                "newText": "_map"
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
