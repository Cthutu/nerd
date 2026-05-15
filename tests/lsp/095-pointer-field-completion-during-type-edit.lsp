FrameInfo :: plex {
    handle u64
    title_heap []
}

FrameSystem :: plex {
    frames [..]FrameInfo
}

impl FrameSystem {
    delete_frame_info :: fn (fs: ^Self, handle: u64) {
        for i, frame in fs.frames {
            on frame.handle == handle => {
                frame.title_heap.
            }
        }
    }
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/completion",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 13,
                "character": 33
            },
            "context": {
                "triggerKind": 2,
                "triggerCharacter": "."
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
                            "line": 3,
                            "character": 0
                        },
                        "end": {
                            "line": 3,
                            "character": 1
                        }
                    },
                    "severity": 1,
                    "code": "0205",
                    "source": "nerd",
                    "message": "Expected declaration or expression but found RightBrace `}`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 3,
                                        "character": 0
                                    },
                                    "end": {
                                        "line": 3,
                                        "character": 1
                                    }
                                }
                            },
                            "message": "help: Expected a type annotation after ':', but found RightBrace `}`"
                        }
                    ]
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 2,
        "result": []
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
