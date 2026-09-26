use std.frame

main :: fn () {
    frame_system := FrameSystem.init()
    defer frame_system.done()

    main_window := {
        system      : frame_system
        id          : NEW_FRAME
        width       : 800
        height      : 600
        title       : "Vulkan Triangle"
        full_screen : no
        resizable   : yes
    }

    frame_system.apply(^main_window)

    -- Main loop
    for frame_system.loop() {
        frame_system.update(^main_window)
        on frame_system.poll(^main_window) {
            None => {
            }

            Resized { width: _, height: _ } => {
            }

            KeyPress { scan_code: _ } => {
            }

            KeyRelease { scan_code: _ } => {}
            Character { codepoint: _ } => {}
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
                "line": 25,
                "character": 22
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
                "line": 25,
                "character": 32
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
                "line": 28,
                "character": 23
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
                "line": 31,
                "character": 25
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
                "line": 32,
                "character": 24
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "method": "textDocument/didChange",
        "params": {
            "textDocument": {
                "uri": "file:///test.n",
                "version": 2
            },
            "contentChanges": [
                {
                    "range": {
                        "start": {
                            "line": 7,
                            "character": 22
                        },
                        "end": {
                            "line": 7,
                            "character": 22
                        }
                    },
                    "text": "^"
                }
            ]
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
                "line": 25,
                "character": 22
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
                "line": 25,
                "character": 32
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 14,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 28,
                "character": 23
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 15,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 31,
                "character": 25
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 16,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 32,
                "character": 24
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
            "diagnostics": [
                {
                    "range": {
                        "start": {
                            "line": 7,
                            "character": 22
                        },
                        "end": {
                            "line": 7,
                            "character": 34
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Type mismatch: expected `^FrameSystem`, found `FrameSystem`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 7,
                                        "character": 22
                                    },
                                    "end": {
                                        "line": 7,
                                        "character": 34
                                    }
                                }
                            },
                            "message": "help: Change the expression or annotation so both sides use the same type."
                        }
                    ]
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 2,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nwidth\n```\n\n- Kind: plex field\n- Type: `u16`\n- Owner: `plex { u16 width, u16 height }`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nheight\n```\n\n- Kind: plex field\n- Type: `u16`\n- Owner: `plex { u16 width, u16 height }`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nscan_code\n```\n\n- Kind: plex field\n- Type: `FrameScanCode`\n- Owner: `plex { FrameScanCode scan_code }`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nscan_code\n```\n\n- Kind: plex field\n- Type: `FrameScanCode`\n- Owner: `plex { FrameScanCode scan_code }`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 6,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\ncodepoint\n```\n\n- Kind: plex field\n- Type: `u32`\n- Owner: `plex { u32 codepoint }`"
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
        "id": 12,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nwidth\n```\n\n- Kind: plex field\n- Type: `u16`\n- Owner: `plex { u16 width, u16 height }`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 13,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nheight\n```\n\n- Kind: plex field\n- Type: `u16`\n- Owner: `plex { u16 width, u16 height }`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 14,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nscan_code\n```\n\n- Kind: plex field\n- Type: `FrameScanCode`\n- Owner: `plex { FrameScanCode scan_code }`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 15,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nscan_code\n```\n\n- Kind: plex field\n- Type: `FrameScanCode`\n- Owner: `plex { FrameScanCode scan_code }`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 16,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\ncodepoint\n```\n\n- Kind: plex field\n- Type: `u32`\n- Owner: `plex { u32 codepoint }`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
