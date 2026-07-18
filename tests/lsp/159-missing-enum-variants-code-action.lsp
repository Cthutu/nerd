use std.frame

main :: fn () {
    system := FrameSystem.init()
    on system.poll() {
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
                    "line": 4,
                    "character": 10
                },
                "end": {
                    "line": 4,
                    "character": 10
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
                            "line": 4,
                            "character": 18
                        },
                        "end": {
                            "line": 4,
                            "character": 19
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Argument count mismatch: expected 1, found 0",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 4,
                                        "character": 18
                                    },
                                    "end": {
                                        "line": 4,
                                        "character": 19
                                    }
                                }
                            },
                            "message": "help: Pass exactly 1 argument to match the function signature."
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
                                        "line": 5,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 5,
                                        "character": 4
                                    }
                                },
                                "newText": "\n        None => {\n        }\n\n        Closed => {\n        }\n\n        Resized { width: _, height: _ } => {\n        }\n\n        KeyPress { scan_code: _ } => {\n        }\n\n        KeyRelease { scan_code: _ } => {\n        }\n\n        Character { codepoint: _ } => {\n        }\n    "
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
