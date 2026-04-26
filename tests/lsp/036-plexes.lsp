use std.print

Point :: plex { x i32 y i32 }

main :: fn () {
    p :: Point { x: 1, y: 2 }
    prn($"{p.x} {p.y}")
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
                "line": 5,
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
                "line": 6,
                "character": 13
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
                "line": 6,
                "character": 13
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
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
                "textDocumentSync": 1,
                "hoverProvider": true,
                "definitionProvider": true,
                "documentSymbolProvider": true,
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
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\np\n```\n\n- Kind: local variable\n- Type: `plex { i32 x, i32 y }`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nx\n```\n\n- Kind: plex field\n- Type: `i32`\n- Owner: `plex { i32 x, i32 y }`"
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
                    "line": 2,
                    "character": 16
                },
                "end": {
                    "line": 2,
                    "character": 17
                }
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "result": {
            "data": [
                0,
                4,
                3,
                4,
                0,
                0,
                3,
                1,
                4,
                0,
                0,
                1,
                5,
                4,
                0,
                2,
                0,
                5,
                4,
                0,
                0,
                9,
                4,
                2,
                0,
                0,
                7,
                1,
                4,
                0,
                0,
                2,
                3,
                4,
                0,
                0,
                4,
                1,
                4,
                0,
                0,
                2,
                3,
                4,
                0,
                0,
                4,
                1,
                4,
                0,
                2,
                0,
                4,
                4,
                0,
                0,
                8,
                2,
                2,
                0,
                0,
                4,
                1,
                4,
                0,
                1,
                4,
                1,
                4,
                0,
                0,
                5,
                5,
                4,
                0,
                0,
                8,
                1,
                4,
                0,
                0,
                3,
                1,
                3,
                0,
                0,
                3,
                1,
                4,
                0,
                0,
                3,
                1,
                3,
                0,
                0,
                2,
                1,
                4,
                0,
                1,
                4,
                3,
                4,
                0,
                0,
                4,
                2,
                5,
                0,
                0,
                3,
                1,
                4,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                1,
                1,
                5,
                0,
                0,
                2,
                1,
                4,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                1,
                1,
                0,
                0,
                0,
                1,
                1,
                4,
                0,
                1,
                0,
                1,
                4,
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
