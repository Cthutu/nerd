Settings :: plex { value i32 }
impl Settings {
    configure :: fn (self: ^Self, first: i32, second: i32 = 2) -> i32 {
        return self.value + first + second
    }
    zero :: fn (self: ^Self) -> i32 { return self.value }
    echo :: fn [T] (self: ^Self, argument: T) -> T {
        _ := self.value
        return argument
    }
}
explicit :: fn (self: ^Settings, first: i32) -> i32 { return self.value + first }
configure :: fn (number: i32) -> i32 { return number }
main :: fn () {
    settings : Settings
    _ := settings.configure(1, second = 2)
    _ := explicit(^settings, 1)
    _ := configure(3)
    _ := settings.zero()
    _ := settings.echo(4)
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
                "line": 15,
                "character": 28
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
                "line": 15,
                "character": 31
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
                "line": 16,
                "character": 18
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
                "line": 17,
                "character": 19
            }
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
                "line": 18,
                "character": 23
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 7,
        "method": "textDocument/signatureHelp",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 19,
                "character": 23
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
                    "text": "Settings :: plex { value i32 }\nimpl Settings {\n    configure :: fn (self: ^Self, first: i32, second: i32 = 2) -> i32 {\n        return self.value + first + second\n    }\n    zero :: fn (self: ^Self) -> i32 { return self.value }\n    echo :: fn [T] (self: ^Self, argument: T) -> T {\n        _ := self.value\n        return argument\n    }\n}\nexplicit :: fn (self: ^Settings, first: i32) -> i32 { return self.value + first }\nconfigure :: fn (number: i32) -> i32 { return number }\nmain :: fn () {\n    settings : Settings\n    _ := settings.configure(1,\n    _ := explicit(^settings, 1)\n    _ := configure(3)\n    _ := settings.zero()\n    _ := settings.echo(4)\n}\n"
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 8,
        "method": "textDocument/signatureHelp",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 15,
                "character": 30
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
                    "label": "configure(first: i32, second: i32 = 2) -> i32",
                    "parameters": [
                        {
                            "label": [
                                10,
                                20
                            ]
                        },
                        {
                            "label": [
                                22,
                                37
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
                    "label": "configure(first: i32, second: i32 = 2) -> i32",
                    "parameters": [
                        {
                            "label": [
                                10,
                                20
                            ]
                        },
                        {
                            "label": [
                                22,
                                37
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
        "id": 4,
        "result": {
            "signatures": [
                {
                    "label": "explicit(self: ^Settings, first: i32) -> i32",
                    "documentation": "Named arguments use `name = value`; omitted parameters use declared defaults when available.",
                    "parameters": [
                        {
                            "label": [
                                9,
                                24
                            ]
                        },
                        {
                            "label": [
                                26,
                                36
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
        "id": 5,
        "result": {
            "signatures": [
                {
                    "label": "configure(number: i32) -> i32",
                    "documentation": "Named arguments use `name = value`; omitted parameters use declared defaults when available.",
                    "parameters": [
                        {
                            "label": [
                                10,
                                21
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
        "id": 6,
        "result": {
            "signatures": [
                {
                    "label": "zero() -> i32",
                    "parameters": []
                }
            ],
            "activeSignature": 0,
            "activeParameter": 0
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 7,
        "result": {
            "signatures": [
                {
                    "label": "echo[T](argument: T) -> T",
                    "parameters": [
                        {
                            "label": [
                                8,
                                19
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
        "method": "textDocument/publishDiagnostics",
        "params": {
            "uri": "file:///test.n",
            "diagnostics": [
                {
                    "range": {
                        "start": {
                            "line": 16,
                            "character": 6
                        },
                        "end": {
                            "line": 16,
                            "character": 7
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Expected RightParen `)` but found Colon `:`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 16,
                                        "character": 6
                                    },
                                    "end": {
                                        "line": 16,
                                        "character": 7
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
        "id": 8,
        "result": {
            "signatures": [
                {
                    "label": "configure(first: i32, second: i32 = 2) -> i32",
                    "parameters": [
                        {
                            "label": [
                                10,
                                20
                            ]
                        },
                        {
                            "label": [
                                22,
                                37
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
