Counter :: plex {
    window_count i32
}

main :: fn () {
    c := Counter { window_count: 1 }
    _ := c.windows_count
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
                    "line": 6,
                    "character": 11
                },
                "end": {
                    "line": 6,
                    "character": 24
                }
            },
            "context": {
                "diagnostics": [
                    {
                        "range": {
                            "start": {
                                "line": 6,
                                "character": 11
                            },
                            "end": {
                                "line": 6,
                                "character": 24
                            }
                        },
                        "severity": 1,
                        "source": "nerd",
                        "message": "Unknown member `windows_count` for `Counter`",
                        "relatedInformation": [
                            {
                                "location": {
                                    "uri": "file:///test.n",
                                    "range": {
                                        "start": {
                                            "line": 6,
                                            "character": 11
                                        },
                                        "end": {
                                            "line": 6,
                                            "character": 24
                                        }
                                    }
                                },
                                "message": "help: Did you mean `.window_count`?"
                            }
                        ]
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
                            "line": 6,
                            "character": 11
                        },
                        "end": {
                            "line": 6,
                            "character": 24
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Unknown member `windows_count` for `Counter`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 6,
                                        "character": 11
                                    },
                                    "end": {
                                        "line": 6,
                                        "character": 24
                                    }
                                }
                            },
                            "message": "help: Did you mean `.window_count`?"
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
                "title": "Change to window_count",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "file:///test.n": [
                            {
                                "range": {
                                    "start": {
                                        "line": 6,
                                        "character": 11
                                    },
                                    "end": {
                                        "line": 6,
                                        "character": 24
                                    }
                                },
                                "newText": "window_count"
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
