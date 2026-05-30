answer :: 41

main :: fn () -> i32 {
    value := answer
    value = value + answer
    return value
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/references",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 4,
                "character": 12
            },
            "context": {
                "includeDeclaration": true
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "method": "textDocument/references",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 4,
                "character": 12
            },
            "context": {
                "includeDeclaration": false
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "method": "textDocument/references",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 4,
                "character": 20
            },
            "context": {
                "includeDeclaration": true
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "method": "textDocument/references",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 4,
                "character": 20
            },
            "context": {
                "includeDeclaration": false
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
            "diagnostics": []
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 2,
        "result": [
            {
                "uri": "file:///test.n",
                "range": {
                    "start": {
                        "line": 3,
                        "character": 4
                    },
                    "end": {
                        "line": 3,
                        "character": 9
                    }
                }
            },
            {
                "uri": "file:///test.n",
                "range": {
                    "start": {
                        "line": 4,
                        "character": 4
                    },
                    "end": {
                        "line": 4,
                        "character": 9
                    }
                }
            },
            {
                "uri": "file:///test.n",
                "range": {
                    "start": {
                        "line": 4,
                        "character": 12
                    },
                    "end": {
                        "line": 4,
                        "character": 17
                    }
                }
            },
            {
                "uri": "file:///test.n",
                "range": {
                    "start": {
                        "line": 5,
                        "character": 11
                    },
                    "end": {
                        "line": 5,
                        "character": 16
                    }
                }
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": [
            {
                "uri": "file:///test.n",
                "range": {
                    "start": {
                        "line": 4,
                        "character": 4
                    },
                    "end": {
                        "line": 4,
                        "character": 9
                    }
                }
            },
            {
                "uri": "file:///test.n",
                "range": {
                    "start": {
                        "line": 4,
                        "character": 12
                    },
                    "end": {
                        "line": 4,
                        "character": 17
                    }
                }
            },
            {
                "uri": "file:///test.n",
                "range": {
                    "start": {
                        "line": 5,
                        "character": 11
                    },
                    "end": {
                        "line": 5,
                        "character": 16
                    }
                }
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": [
            {
                "uri": "file:///test.n",
                "range": {
                    "start": {
                        "line": 0,
                        "character": 0
                    },
                    "end": {
                        "line": 0,
                        "character": 6
                    }
                }
            },
            {
                "uri": "file:///test.n",
                "range": {
                    "start": {
                        "line": 3,
                        "character": 13
                    },
                    "end": {
                        "line": 3,
                        "character": 19
                    }
                }
            },
            {
                "uri": "file:///test.n",
                "range": {
                    "start": {
                        "line": 4,
                        "character": 20
                    },
                    "end": {
                        "line": 4,
                        "character": 26
                    }
                }
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "result": [
            {
                "uri": "file:///test.n",
                "range": {
                    "start": {
                        "line": 3,
                        "character": 13
                    },
                    "end": {
                        "line": 3,
                        "character": 19
                    }
                }
            },
            {
                "uri": "file:///test.n",
                "range": {
                    "start": {
                        "line": 4,
                        "character": 20
                    },
                    "end": {
                        "line": 4,
                        "character": 26
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
