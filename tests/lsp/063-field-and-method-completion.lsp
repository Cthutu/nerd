Point :: plex {
    x i32
    y i32
}

Counter :: plex {
    use Point
    value i32
}

impl Counter {
    inc :: fn (self: ^Self, amount: i32) {
        self.value += amount
    }

    get :: fn (self: Self) -> i32 {
        return self.value
    }
}

main :: fn () {
    counter: Counter
    _ := counter.x
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
                "line": 22,
                "character": 17
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
                "line": 22,
                "character": 17
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "method": "textDocument/definition",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 22,
                "character": 17
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
                "label": "x",
                "kind": 5,
                "detail": "field"
            },
            {
                "label": "y",
                "kind": 5,
                "detail": "field"
            },
            {
                "label": "value",
                "kind": 5,
                "detail": "field"
            },
            {
                "label": "inc",
                "kind": 2,
                "detail": "method"
            },
            {
                "label": "get",
                "kind": 2,
                "detail": "method"
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nx\n```\n\n- Kind: plex field\n- Type: `i32`\n- Owner: `Counter`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": {
            "uri": "file:///test.n",
            "range": {
                "start": {
                    "line": 1,
                    "character": 4
                },
                "end": {
                    "line": 1,
                    "character": 5
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
