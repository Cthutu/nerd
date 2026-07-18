memory :: use std.mem

main :: fn () {
    _ := memory.kb(2)
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
                "line": 0,
                "character": 0
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
                "character": 16
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
                "character": 16
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "method": "textDocument/definition",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 0,
                "character": 15
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
                "value": "```nerd\nmemory\n```\n\n- Kind: module\n- Type: `module`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nkb :: fn (n: usize) -> usize\n```\n\n- Kind: function\n\nConverts kibibytes to bytes."
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": {
            "uri": "file:///home/matt/nerd/mods/std/mem.n",
            "range": {
                "start": {
                    "line": 30,
                    "character": 4
                },
                "end": {
                    "line": 30,
                    "character": 6
                }
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "result": {
            "uri": "file:///home/matt/nerd/mods/std/mem.n",
            "range": {
                "start": {
                    "line": 0,
                    "character": 0
                },
                "end": {
                    "line": 0,
                    "character": 0
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
