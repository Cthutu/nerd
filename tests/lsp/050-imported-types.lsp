use test.lsp_types

main :: fn () {
    _p : Point  = Point { x: 1, y: 2 }
    _c : Colour = Colour.RED
    _v : Value  = Value { i: 1 }
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/definition",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 3,
                "character": 9
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "method": "textDocument/definition",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 4,
                "character": 9
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
                "line": 5,
                "character": 9
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
            "uri": "__REPO_URI__/tests/mods/test/lsp_types.n",
            "range": {
                "start": {
                    "line": 0,
                    "character": 4
                },
                "end": {
                    "line": 0,
                    "character": 9
                }
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "uri": "__REPO_URI__/tests/mods/test/lsp_types.n",
            "range": {
                "start": {
                    "line": 5,
                    "character": 4
                },
                "end": {
                    "line": 5,
                    "character": 10
                }
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": {
            "uri": "__REPO_URI__/tests/mods/test/lsp_types.n",
            "range": {
                "start": {
                    "line": 10,
                    "character": 4
                },
                "end": {
                    "line": 10,
                    "character": 9
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
