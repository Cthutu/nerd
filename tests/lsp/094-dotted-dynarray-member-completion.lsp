FrameInfo :: plex {
    handle u64
}

FrameSystem :: plex {
    frames [..]FrameInfo
}

impl FrameSystem {
    done :: fn (fs: ^Self) {
        fs.frames.
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
                "line": 10,
                "character": 18
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
                            "line": 11,
                            "character": 4
                        },
                        "end": {
                            "line": 11,
                            "character": 5
                        }
                    },
                    "severity": 1,
                    "code": "0203",
                    "source": "nerd",
                    "message": "Expected Symbol but found RightBrace `}`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 11,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 11,
                                        "character": 5
                                    }
                                }
                            },
                            "message": "help: Check for a missing closing delimiter or misplaced operator"
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
                "label": "data",
                "kind": 5
            },
            {
                "label": "count",
                "kind": 5
            },
            {
                "label": "capacity",
                "kind": 5
            },
            {
                "label": "append",
                "kind": 2
            },
            {
                "label": "clear",
                "kind": 2
            },
            {
                "label": "delete",
                "kind": 2
            },
            {
                "label": "free",
                "kind": 2
            },
            {
                "label": "pop",
                "kind": 2
            },
            {
                "label": "push",
                "kind": 2
            },
            {
                "label": "reserve_to",
                "kind": 2
            },
            {
                "label": "reserve_extra",
                "kind": 2
            },
            {
                "label": "resize_to",
                "kind": 2
            },
            {
                "label": "swap_delete",
                "kind": 2
            },
            {
                "label": "resize_undefined_to",
                "kind": 2
            },
            {
                "label": "extend",
                "kind": 2
            },
            {
                "label": "extend_undefined",
                "kind": 2
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
