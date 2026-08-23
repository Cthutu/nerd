-- Core bindings are implicit.

Owner :: plex {
    scratch arena
    broken : string -- Keep completion available after an earlier error.
}

main :: fn () {
    arena_var := arena(16, 8)
    _ := arena_var.

    text := "hello"
    _ := text.
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
                "line": 9,
                "character": 19
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "method": "textDocument/completion",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 12,
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
                            "line": 4,
                            "character": 11
                        },
                        "end": {
                            "line": 4,
                            "character": 12
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Expected type but found Colon `:`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 4,
                                        "character": 11
                                    },
                                    "end": {
                                        "line": 4,
                                        "character": 12
                                    }
                                }
                            },
                            "message": "note: Plex field definitions are written as `field Type`."
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 4,
                                        "character": 11
                                    },
                                    "end": {
                                        "line": 4,
                                        "character": 12
                                    }
                                }
                            },
                            "message": "help: Remove the colon. Colons are used in plex literals such as `State { loc_index: 0 }`, not in plex definitions."
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
                "label": "pr",
                "kind": 2,
                "detail": "method"
            },
            {
                "label": "prn",
                "kind": 2,
                "detail": "method"
            },
            {
                "label": "alloc",
                "kind": 2,
                "detail": "method"
            },
            {
                "label": "alloc_array",
                "kind": 2,
                "detail": "method"
            },
            {
                "label": "alloc_bytes",
                "kind": 2,
                "detail": "method"
            },
            {
                "label": "reset",
                "kind": 2,
                "detail": "method"
            },
            {
                "label": "mark",
                "kind": 2,
                "detail": "method"
            },
            {
                "label": "restore",
                "kind": 2,
                "detail": "method"
            },
            {
                "label": "done",
                "kind": 2,
                "detail": "method"
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": [
            {
                "label": "data",
                "kind": 5,
                "detail": "field"
            },
            {
                "label": "count",
                "kind": 5,
                "detail": "field"
            },
            {
                "label": "size",
                "kind": 5,
                "detail": "field"
            },
            {
                "label": "bytes",
                "kind": 5,
                "detail": "field"
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
