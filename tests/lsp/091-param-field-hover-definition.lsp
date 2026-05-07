FrameSystem :: plex {
    frames [..]i32
}

main :: fn () {
}

use_frame :: fn (fs: ^FrameSystem) {
    fs.frames
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
                "line": 8,
                "character": 7
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
                "line": 8,
                "character": 7
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
                        "."
                    ],
                    "resolveProvider": false
                },
                "signatureHelpProvider": {
                    "triggerCharacters": [
                        "(",
                        ","
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
            "diagnostics": [
                {
                    "range": {
                        "start": {
                            "line": 8,
                            "character": 4
                        },
                        "end": {
                            "line": 8,
                            "character": 6
                        }
                    },
                    "severity": 1,
                    "code": "0345",
                    "source": "nerd",
                    "message": "Expression result of type `[..]i32` is not used",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 8,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 8,
                                        "character": 6
                                    }
                                }
                            },
                            "message": "note: Only `void` expressions can be used as standalone statements."
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 8,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 8,
                                        "character": 6
                                    }
                                }
                            },
                            "message": "help: Bind the result to `_` when the value is intentionally ignored."
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
                "value": "```nerd\nframes\n```\n\n- Kind: plex field\n- Type: `[..]i32`\n- Owner: `FrameSystem`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "uri": "file:///test.n",
            "range": {
                "start": {
                    "line": 1,
                    "character": 4
                },
                "end": {
                    "line": 1,
                    "character": 10
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
