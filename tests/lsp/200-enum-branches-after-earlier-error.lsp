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
        }
    }
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/codeAction",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "range": {
                "start": {
                    "line": 21,
                    "character": 28
                },
                "end": {
                    "line": 21,
                    "character": 28
                }
            },
            "context": {
                "diagnostics": []
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "method": "textDocument/codeAction",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "range": {
                "start": {
                    "line": 22,
                    "character": 8
                },
                "end": {
                    "line": 22,
                    "character": 8
                }
            },
            "context": {
                "diagnostics": []
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "method": "textDocument/codeAction",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "range": {
                "start": {
                    "line": 21,
                    "character": 9
                },
                "end": {
                    "line": 21,
                    "character": 9
                }
            },
            "context": {
                "diagnostics": []
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
                            "line": 22,
                            "character": 8
                        },
                        "end": {
                            "line": 22,
                            "character": 8
                        }
                    },
                    "text": "    None => {}\n        "
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "method": "textDocument/codeAction",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "range": {
                "start": {
                    "line": 21,
                    "character": 28
                },
                "end": {
                    "line": 21,
                    "character": 28
                }
            },
            "context": {
                "diagnostics": []
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
        "id": 6,
        "method": "textDocument/codeAction",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "range": {
                "start": {
                    "line": 21,
                    "character": 28
                },
                "end": {
                    "line": 21,
                    "character": 28
                }
            },
            "context": {
                "diagnostics": []
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
        "result": [
            {
                "title": "Add missing enum variants",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "file:///test.n": [
                            {
                                "range": {
                                    "start": {
                                        "line": 22,
                                        "character": 8
                                    },
                                    "end": {
                                        "line": 22,
                                        "character": 8
                                    }
                                },
                                "newText": "\n            None => {\n            }\n\n            Closed => {\n            }\n\n            Resized { width: _, height: _ } => {\n            }\n\n            KeyPress { scan_code: _ } => {\n            }\n\n            KeyRelease { scan_code: _ } => {\n            }\n\n            Character { codepoint: _ } => {\n            }\n        "
                            }
                        ]
                    }
                }
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": [
            {
                "title": "Add missing enum variants",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "file:///test.n": [
                            {
                                "range": {
                                    "start": {
                                        "line": 22,
                                        "character": 8
                                    },
                                    "end": {
                                        "line": 22,
                                        "character": 8
                                    }
                                },
                                "newText": "\n            None => {\n            }\n\n            Closed => {\n            }\n\n            Resized { width: _, height: _ } => {\n            }\n\n            KeyPress { scan_code: _ } => {\n            }\n\n            KeyRelease { scan_code: _ } => {\n            }\n\n            Character { codepoint: _ } => {\n            }\n        "
                            }
                        ]
                    }
                }
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": [
            {
                "title": "Add missing enum variants",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "file:///test.n": [
                            {
                                "range": {
                                    "start": {
                                        "line": 22,
                                        "character": 8
                                    },
                                    "end": {
                                        "line": 22,
                                        "character": 8
                                    }
                                },
                                "newText": "\n            None => {\n            }\n\n            Closed => {\n            }\n\n            Resized { width: _, height: _ } => {\n            }\n\n            KeyPress { scan_code: _ } => {\n            }\n\n            KeyRelease { scan_code: _ } => {\n            }\n\n            Character { codepoint: _ } => {\n            }\n        "
                            }
                        ]
                    }
                }
            }
        ]
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
        "id": 5,
        "result": [
            {
                "title": "Add missing enum variants",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "file:///test.n": [
                            {
                                "range": {
                                    "start": {
                                        "line": 23,
                                        "character": 8
                                    },
                                    "end": {
                                        "line": 23,
                                        "character": 8
                                    }
                                },
                                "newText": "\n            Closed => {\n            }\n\n            Resized { width: _, height: _ } => {\n            }\n\n            KeyPress { scan_code: _ } => {\n            }\n\n            KeyRelease { scan_code: _ } => {\n            }\n\n            Character { codepoint: _ } => {\n            }\n        "
                            }
                        ]
                    }
                }
            }
        ]
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
        "id": 6,
        "result": [
            {
                "title": "Add missing enum variants",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "file:///test.n": [
                            {
                                "range": {
                                    "start": {
                                        "line": 23,
                                        "character": 8
                                    },
                                    "end": {
                                        "line": 23,
                                        "character": 8
                                    }
                                },
                                "newText": "\n            Closed => {\n            }\n\n            Resized { width: _, height: _ } => {\n            }\n\n            KeyPress { scan_code: _ } => {\n            }\n\n            KeyRelease { scan_code: _ } => {\n            }\n\n            Character { codepoint: _ } => {\n            }\n        "
                            }
                        ]
                    }
                }
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
