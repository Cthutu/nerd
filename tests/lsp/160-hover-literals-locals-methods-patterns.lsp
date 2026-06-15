FrameSystem :: plex {
    id i32
}

Frame :: plex {
    system ^FrameSystem
    id i32
}

impl FrameSystem {
    apply :: fn (self: ^Self, frame: ^Frame) {
        _ := self
        _ := frame
    }
}

Event :: enum {
    Closed
    KeyPress { scan_code i32 }
}

main :: fn () {
    frame_system := FrameSystem { id: 1 }
    frame := {
        system : ^frame_system
        id     : 2
    }

    frame_system.apply(^frame)

    event := Event.KeyPress { scan_code: 27 }
    on event {
        Closed => {
        }
        KeyPress { scan_code: code } => {
            _ := code
        }
    }
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
                "line": 22,
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
                "line": 23,
                "character": 4
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
                "line": 24,
                "character": 8
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
                "line": 25,
                "character": 8
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 6,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 28,
                "character": 4
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 7,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 28,
                "character": 17
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 8,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 28,
                "character": 24
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 9,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 32,
                "character": 8
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 10,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 34,
                "character": 8
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 11,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 34,
                "character": 19
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 12,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 34,
                "character": 30
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 13,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 35,
                "character": 18
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
                "value": "```nerd\nframe_system\n```\n\n- Kind: local variable\n- Type: `FrameSystem`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nframe\n```\n\n- Kind: local variable\n- Type: `Frame`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nsystem\n```\n\n- Kind: plex field\n- Type: `^FrameSystem`\n- Owner: `Frame`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nid\n```\n\n- Kind: plex field\n- Type: `i32`\n- Owner: `Frame`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 6,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nframe_system\n```\n\n- Kind: local variable\n- Type: `FrameSystem`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 7,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\napply :: fn (self: ^Self, frame: ^Frame) -> void\n```\n\n- Kind: method"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 8,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nframe\n```\n\n- Kind: local variable\n- Type: `Frame`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 9,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nClosed\n```\n\n- Kind: enum variant\n- Owner: `Event`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 10,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nKeyPress\n```\n\n- Kind: enum variant\n- Owner: `Event`\n- Payload: `plex { i32 scan_code }`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 11,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nscan_code\n```\n\n- Kind: plex field\n- Type: `i32`\n- Owner: `plex { i32 scan_code }`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 12,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\ncode\n```\n\n- Kind: pattern binder\n- Type: `i32`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 13,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\ncode\n```\n\n- Kind: pattern binder\n- Type: `i32`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
