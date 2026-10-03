use std.raptor
square :: fn (number: ^i32) -> i32 { return number^ * number^ }
main :: fn () -> ?void {
    scheduler : Scheduler
    scheduler.init(2)?
    queue := scheduler.get_default_queue()
    number : i32 = 4
    task := queue.async(square, ^number)?
    defer assert task.done()
    _ := task.get()
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
                "character": 19
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
                "line": 7,
                "character": 24
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "method": "textDocument/signatureHelp",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 7,
                "character": 32
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "method": "textDocument/signatureHelp",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 9,
                "character": 18
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
                    "text": "use std.raptor\nsquare :: fn (number: ^i32) -> i32 { return number^ * number^ }\nmain :: fn () -> ?void {\n    scheduler : Scheduler\n    scheduler.init(2)?\n    queue := scheduler.get_default_queue()\n    number : i32 = 4\n    task := queue.async(square,\n    defer assert task.done()\n    _ := task.get()\n    assert scheduler.close()\n    return\n}\n"
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 6,
        "method": "textDocument/signatureHelp",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 7,
                "character": 31
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
            "signatures": [
                {
                    "label": "init(workers: usize, capacity: usize = 256) -> ?void",
                    "parameters": [
                        {
                            "label": [
                                5,
                                19
                            ]
                        },
                        {
                            "label": [
                                21,
                                42
                            ]
                        }
                    ]
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
                    "label": "async[A, T](callback: fn (argument: A) -> T, argument: A) -> ?Task[T]",
                    "parameters": [
                        {
                            "label": [
                                12,
                                43
                            ]
                        },
                        {
                            "label": [
                                45,
                                56
                            ]
                        }
                    ]
                }
            ],
            "activeSignature": 0,
            "activeParameter": 0
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": {
            "signatures": [
                {
                    "label": "async[A, T](callback: fn (argument: A) -> T, argument: A) -> ?Task[T]",
                    "parameters": [
                        {
                            "label": [
                                12,
                                43
                            ]
                        },
                        {
                            "label": [
                                45,
                                56
                            ]
                        }
                    ]
                }
            ],
            "activeSignature": 0,
            "activeParameter": 1
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "result": {
            "signatures": [
                {
                    "label": "get() -> ?T",
                    "parameters": []
                }
            ],
            "activeSignature": 0,
            "activeParameter": 0
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
                            "character": 9
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Missing value before Keyword `defer`",
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
                                        "character": 9
                                    }
                                }
                            },
                            "message": "help: Insert a literal, parenthesized expression, or unary operator"
                        }
                    ]
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 6,
        "result": {
            "signatures": [
                {
                    "label": "async[A, T](callback: fn (argument: A) -> T, argument: A) -> ?Task[T]",
                    "parameters": [
                        {
                            "label": [
                                12,
                                43
                            ]
                        },
                        {
                            "label": [
                                45,
                                56
                            ]
                        }
                    ]
                }
            ],
            "activeSignature": 0,
            "activeParameter": 1
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
