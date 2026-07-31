Counter :: plex {
    value i32
}

impl Counter {
    -- Increases the counter by `amount`.
    inc :: fn (self: ^Self, amount: i32) {
        self.value += amount
    }
}

get_size :: fn () -> (i32, i32) {
    return (80, 25)
}

test "tuple hover inside source test" {
    size := get_size()

    assert size.0 >= 0
    counter := Counter { value: 0 }
    counter.inc(1)
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 16,
                "character": 4
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
                "line": 18,
                "character": 11
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
                "line": 18,
                "character": 16
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
                "line": 20,
                "character": 13
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
                "line": 16,
                "character": 14
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
                "line": 20,
                "character": 13
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
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nsize\n```\n\n- Kind: local variable\n- Type: `(i32, i32)`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nsize\n```\n\n- Kind: local variable\n- Type: `(i32, i32)`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\n0\n```\n\n- Kind: tuple field\n- Type: `i32`\n- Owner: `(i32, i32)`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\ninc :: fn (self: ^Self, amount: i32) -> void\n```\n\n- Kind: method\n\nIncreases the counter by `amount`."
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
                    "line": 11,
                    "character": 0
                },
                "end": {
                    "line": 11,
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
                    "line": 6,
                    "character": 4
                },
                "end": {
                    "line": 6,
                    "character": 7
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
