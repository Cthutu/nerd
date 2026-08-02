-- Core bindings are implicit.

main :: fn () {
    scratch: arena
    scratch = arena(16)
    _ := scratch.mark()
    temp_arena.reset()
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
                "line": 3,
                "character": 13
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
                "line": 5,
                "character": 17
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
                "line": 6,
                "character": 4
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
                "line": 6,
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
                "value": "```nerd\narena\n```\n\n```nerd\narena :: fn (num_bytes: usize, increment: usize = num_bytes) -> arena\n```\n\n- Kind: built-in type\n- Type: `arena`\n- Notes: opaque, pointer-stable allocation arena\n\nCreates a pointer-stable arena with the requested initial capacity. The optional increment controls how much storage is committed when more space is needed."
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nmark :: fn () -> u32\n```\n\n- Kind: arena method\n- Owner: `arena`\n\nReturns the current arena cursor for a later restore operation."
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\ntemp_arena\n```\n\n- Kind: constant\n- Type: `^arena`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nreset :: fn () -> void\n```\n\n- Kind: arena method\n- Owner: `arena`\n\nInvalidates all allocations from the arena while retaining its storage for reuse."
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
