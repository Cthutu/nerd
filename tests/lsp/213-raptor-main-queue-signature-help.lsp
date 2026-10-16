use std.raptor
main :: fn () -> ?void {
    scheduler : Scheduler
    scheduler.init(2)?
    queue := scheduler.get_main_queue()
    _ := scheduler.run_main_tasks()
    assert scheduler.close()
    return
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/signatureHelp",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 4,
                "character": 38
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "method": "textDocument/signatureHelp",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 5,
                "character": 34
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
                            "character": 4
                        },
                        "end": {
                            "line": 4,
                            "character": 9
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Unused local variable `queue`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 4,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 4,
                                        "character": 9
                                    }
                                }
                            },
                            "message": "note: Assigning to a variable does not count as using it."
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 4,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 4,
                                        "character": 9
                                    }
                                }
                            },
                            "message": "help: Remove `queue` or prefix the name with `_` if it is deliberately unused."
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
            "signatures": [
                {
                    "label": "get_main_queue() -> ^Queue",
                    "parameters": []
                }
            ],
            "activeSignature": 0,
            "activeParameter": 0
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "signatures": [
                {
                    "label": "run_main_tasks() -> ?usize",
                    "parameters": []
                }
            ],
            "activeSignature": 0,
            "activeParameter": 0
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
