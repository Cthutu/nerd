Display :: trait {
    show :: fn (Self) -> string
}

Point :: plex { x i32 y i32 }

impl Display for Point {
    show :: fn (self: Self) -> string {
        return $"({self.x}, {self.y})"
    }
}

main :: fn () {
    p := Point { x: 1, y: 2 }
    prn(p.show())
    prn(Display.show(p))
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/documentSymbol",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 0,
                "character": 1
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 1,
                "character": 5
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 7,
                "character": 5
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 6,
        "method": "textDocument/definition",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 14,
                "character": 11
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 7,
        "method": "textDocument/definition",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 15,
                "character": 17
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 8,
        "method": "textDocument/semanticTokens/full",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
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
            "diagnostics": []
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 2,
        "result": [
            {
                "name": "Display",
                "kind": 11,
                "range": {
                    "start": {
                        "line": 0,
                        "character": 0
                    },
                    "end": {
                        "line": 0,
                        "character": 7
                    }
                },
                "selectionRange": {
                    "start": {
                        "line": 0,
                        "character": 0
                    },
                    "end": {
                        "line": 0,
                        "character": 7
                    }
                },
                "detail": "trait"
            },
            {
                "name": "Point",
                "kind": 14,
                "range": {
                    "start": {
                        "line": 4,
                        "character": 0
                    },
                    "end": {
                        "line": 4,
                        "character": 5
                    }
                },
                "selectionRange": {
                    "start": {
                        "line": 4,
                        "character": 0
                    },
                    "end": {
                        "line": 4,
                        "character": 5
                    }
                },
                "detail": "type alias"
            },
            {
                "name": "impl Display for Point",
                "kind": 11,
                "range": {
                    "start": {
                        "line": 6,
                        "character": 0
                    },
                    "end": {
                        "line": 6,
                        "character": 4
                    }
                },
                "selectionRange": {
                    "start": {
                        "line": 6,
                        "character": 0
                    },
                    "end": {
                        "line": 6,
                        "character": 4
                    }
                },
                "detail": "impl"
            },
            {
                "name": "main",
                "kind": 12,
                "range": {
                    "start": {
                        "line": 12,
                        "character": 0
                    },
                    "end": {
                        "line": 12,
                        "character": 4
                    }
                },
                "selectionRange": {
                    "start": {
                        "line": 12,
                        "character": 0
                    },
                    "end": {
                        "line": 12,
                        "character": 4
                    }
                },
                "detail": "function"
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nDisplay :: trait\n```\n\n- Kind: trait"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nshow :: fn (Self) -> string\n```\n\n- Kind: trait member"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nshow :: fn (self: Self) -> string\n```\n\n- Kind: impl method"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 6,
        "result": {
            "uri": "file:///test.n",
            "range": {
                "start": {
                    "line": 7,
                    "character": 4
                },
                "end": {
                    "line": 7,
                    "character": 8
                }
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 7,
        "result": {
            "uri": "file:///test.n",
            "range": {
                "start": {
                    "line": 7,
                    "character": 4
                },
                "end": {
                    "line": 7,
                    "character": 8
                }
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 8,
        "result": {
            "data": [
                0,
                0,
                7,
                0,
                0,
                0,
                8,
                1,
                4,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                2,
                5,
                2,
                0,
                1,
                4,
                4,
                1,
                0,
                0,
                5,
                1,
                4,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                2,
                2,
                2,
                0,
                0,
                4,
                4,
                2,
                0,
                0,
                6,
                2,
                4,
                0,
                0,
                3,
                6,
                0,
                0,
                3,
                0,
                5,
                0,
                0,
                0,
                6,
                1,
                4,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                2,
                4,
                2,
                0,
                0,
                7,
                1,
                0,
                0,
                0,
                2,
                3,
                0,
                0,
                0,
                4,
                1,
                0,
                0,
                0,
                2,
                3,
                0,
                0,
                2,
                0,
                4,
                2,
                0,
                0,
                5,
                7,
                0,
                0,
                0,
                8,
                3,
                2,
                0,
                0,
                4,
                5,
                0,
                0,
                1,
                4,
                4,
                1,
                0,
                0,
                5,
                1,
                4,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                2,
                2,
                2,
                0,
                0,
                4,
                4,
                0,
                0,
                0,
                4,
                1,
                4,
                0,
                0,
                2,
                4,
                2,
                0,
                0,
                6,
                2,
                4,
                0,
                0,
                3,
                6,
                0,
                0,
                1,
                8,
                6,
                2,
                0,
                0,
                7,
                2,
                5,
                0,
                0,
                2,
                1,
                5,
                0,
                0,
                2,
                4,
                0,
                0,
                0,
                4,
                1,
                4,
                0,
                0,
                1,
                1,
                0,
                0,
                0,
                2,
                2,
                5,
                0,
                0,
                3,
                4,
                0,
                0,
                0,
                4,
                1,
                4,
                0,
                0,
                1,
                1,
                0,
                0,
                0,
                2,
                2,
                5,
                0,
                0,
                1,
                1,
                5,
                0,
                4,
                0,
                4,
                1,
                0,
                0,
                5,
                1,
                4,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                2,
                2,
                2,
                0,
                1,
                4,
                1,
                0,
                0,
                0,
                2,
                1,
                4,
                0,
                0,
                3,
                5,
                0,
                0,
                0,
                8,
                1,
                0,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                2,
                1,
                3,
                0,
                0,
                3,
                1,
                0,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                2,
                1,
                3,
                0,
                1,
                4,
                3,
                1,
                0,
                0,
                4,
                1,
                0,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                1,
                4,
                1,
                0,
                1,
                4,
                3,
                1,
                0,
                0,
                4,
                7,
                0,
                0,
                0,
                7,
                1,
                4,
                0,
                0,
                1,
                4,
                1,
                0,
                0,
                5,
                1,
                0,
                0
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
