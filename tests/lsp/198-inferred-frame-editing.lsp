use std.frame

main :: fn () {
    frame_system := FrameSystem.init()
    defer frame_system.done()

    main_window := {
    }

    frame_system.apply(^main_window)
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
                "character": 8
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
                "character": 33
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
                "line": 4,
                "character": 24
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
                "line": 9,
                "character": 20
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 7,
        "method": "textDocument/codeAction",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "range": {
                "start": {
                    "line": 6,
                    "character": 20
                },
                "end": {
                    "line": 6,
                    "character": 20
                }
            },
            "context": {
                "diagnostics": []
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
                "line": 9,
                "character": 27
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
                "line": 4,
                "character": 14
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
                            "character": 4
                        },
                        "end": {
                            "line": 7,
                            "character": 4
                        }
                    },
                    "text": "..."
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
                "line": 3,
                "character": 8
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
                "line": 3,
                "character": 33
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
                "line": 4,
                "character": 24
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
                "line": 6,
                "character": 8
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
                "line": 9,
                "character": 20
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 17,
        "method": "textDocument/codeAction",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "range": {
                "start": {
                    "line": 6,
                    "character": 20
                },
                "end": {
                    "line": 6,
                    "character": 20
                }
            },
            "context": {
                "diagnostics": []
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 18,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 9,
                "character": 27
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 19,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 4,
                "character": 14
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "method": "textDocument/didChange",
        "params": {
            "textDocument": {
                "uri": "file:///test.n",
                "version": 3
            },
            "contentChanges": [
                {
                    "range": {
                        "start": {
                            "line": 7,
                            "character": 4
                        },
                        "end": {
                            "line": 7,
                            "character": 7
                        }
                    },
                    "text": ""
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 22,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 3,
                "character": 8
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 23,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 3,
                "character": 33
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 24,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 4,
                "character": 24
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 25,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 6,
                "character": 8
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 26,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 9,
                "character": 20
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 27,
        "method": "textDocument/codeAction",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "range": {
                "start": {
                    "line": 6,
                    "character": 20
                },
                "end": {
                    "line": 6,
                    "character": 20
                }
            },
            "context": {
                "diagnostics": []
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 28,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 9,
                "character": 27
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 29,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 4,
                "character": 14
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
                            "line": 6,
                            "character": 19
                        },
                        "end": {
                            "line": 6,
                            "character": 20
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Plex literal is missing required fields",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 6,
                                        "character": 19
                                    },
                                    "end": {
                                        "line": 6,
                                        "character": 20
                                    }
                                }
                            },
                            "message": "note: Missing fields: `system`, `id`, `width`, `height`, `title`, `full_screen`, `resizable`"
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 6,
                                        "character": 19
                                    },
                                    "end": {
                                        "line": 6,
                                        "character": 20
                                    }
                                }
                            },
                            "message": "help: Add all fields required by the plex type, or write `...` in the literal to initialise omitted fields with their default values."
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
                "value": "```nerd\ninit :: fn () -> Self\n```\n\n- Kind: method\n\nCreates an empty frame system ready to allocate frame IDs."
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\ndone :: fn (system: ^Self) -> void\n```\n\n- Kind: method\n\nDestroys any remaining frames and releases all storage owned by the\nsystem. Calling `done` more than once is safe."
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nmain_window\n```\n\n- Kind: local variable\n- Type: `Frame`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 6,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\napply :: fn (system: ^Self, frame: ^Frame) -> void\n```\n\n- Kind: method"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 7,
        "result": [
            {
                "title": "Fill missing plex fields",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "file:///test.n": [
                            {
                                "range": {
                                    "start": {
                                        "line": 6,
                                        "character": 20
                                    },
                                    "end": {
                                        "line": 7,
                                        "character": 4
                                    }
                                },
                                "newText": "\n        system     : nil\n        id         : 0\n        width      : 0\n        height     : 0\n        title      : \"\"\n        full_screen: no\n        resizable  : no\n    "
                            }
                        ]
                    }
                }
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 8,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nmain_window\n```\n\n- Kind: local variable\n- Type: `Frame`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 9,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nframe_system\n```\n\n- Kind: local variable\n- Type: `FrameSystem`"
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
                "value": "```nerd\nframe_system\n```\n\n- Kind: local variable\n- Type: `FrameSystem`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 13,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\ninit :: fn () -> Self\n```\n\n- Kind: method\n\nCreates an empty frame system ready to allocate frame IDs."
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 14,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\ndone :: fn (system: ^Self) -> void\n```\n\n- Kind: method\n\nDestroys any remaining frames and releases all storage owned by the\nsystem. Calling `done` more than once is safe."
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 15,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nmain_window\n```\n\n- Kind: local variable\n- Type: `Frame`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 16,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\napply :: fn (system: ^Self, frame: ^Frame) -> void\n```\n\n- Kind: method"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 17,
        "result": [
            {
                "title": "Fill missing plex fields",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "file:///test.n": [
                            {
                                "range": {
                                    "start": {
                                        "line": 6,
                                        "character": 20
                                    },
                                    "end": {
                                        "line": 7,
                                        "character": 7
                                    }
                                },
                                "newText": "\n        system     : nil\n        id         : 0\n        width      : 0\n        height     : 0\n        title      : \"\"\n        full_screen: no\n        resizable  : no\n    "
                            }
                        ]
                    }
                }
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 18,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nmain_window\n```\n\n- Kind: local variable\n- Type: `Frame`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 19,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nframe_system\n```\n\n- Kind: local variable\n- Type: `FrameSystem`"
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
                            "line": 6,
                            "character": 19
                        },
                        "end": {
                            "line": 6,
                            "character": 20
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Plex literal is missing required fields",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 6,
                                        "character": 19
                                    },
                                    "end": {
                                        "line": 6,
                                        "character": 20
                                    }
                                }
                            },
                            "message": "note: Missing fields: `system`, `id`, `width`, `height`, `title`, `full_screen`, `resizable`"
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 6,
                                        "character": 19
                                    },
                                    "end": {
                                        "line": 6,
                                        "character": 20
                                    }
                                }
                            },
                            "message": "help: Add all fields required by the plex type, or write `...` in the literal to initialise omitted fields with their default values."
                        }
                    ]
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 22,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nframe_system\n```\n\n- Kind: local variable\n- Type: `FrameSystem`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 23,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\ninit :: fn () -> Self\n```\n\n- Kind: method\n\nCreates an empty frame system ready to allocate frame IDs."
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 24,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\ndone :: fn (system: ^Self) -> void\n```\n\n- Kind: method\n\nDestroys any remaining frames and releases all storage owned by the\nsystem. Calling `done` more than once is safe."
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 25,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nmain_window\n```\n\n- Kind: local variable\n- Type: `Frame`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 26,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\napply :: fn (system: ^Self, frame: ^Frame) -> void\n```\n\n- Kind: method"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 27,
        "result": [
            {
                "title": "Fill missing plex fields",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "file:///test.n": [
                            {
                                "range": {
                                    "start": {
                                        "line": 6,
                                        "character": 20
                                    },
                                    "end": {
                                        "line": 7,
                                        "character": 4
                                    }
                                },
                                "newText": "\n        system     : nil\n        id         : 0\n        width      : 0\n        height     : 0\n        title      : \"\"\n        full_screen: no\n        resizable  : no\n    "
                            }
                        ]
                    }
                }
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 28,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nmain_window\n```\n\n- Kind: local variable\n- Type: `Frame`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 29,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nframe_system\n```\n\n- Kind: local variable\n- Type: `FrameSystem`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
