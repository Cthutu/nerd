main :: fn () {
    items: [4..]string = ["north", "south"]
    items.push("east")
    _cap := items.capacity
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
                "line": 1,
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
                "line": 3,
                "character": 18
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
                "line": 3,
                "character": 12
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
                "value": "```nerd\nitems\n```\n\n- Kind: local variable\n- Type: `[..]string`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\ncapacity\n```\n\n- Kind: dynamic array field\n- Type: `usize`\n- Owner: `[..]string`"
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
