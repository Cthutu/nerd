-- test-platform: windows
ffi "user32" {
    GetModuleHandle ()
}

main :: fn () {}
¬
[]
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
                            "line": 2,
                            "character": 4
                        },
                        "end": {
                            "line": 2,
                            "character": 19
                        }
                    },
                    "severity": 1,
                    "code": "0346",
                    "source": "nerd",
                    "message": "Foreign symbol `GetModuleHandle` was not found in `user32`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 2,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 2,
                                        "character": 19
                                    }
                                }
                            },
                            "message": "note: Windows API names that are macros in C headers often need an explicit `A` or `W` foreign symbol name in Nerd."
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 2,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 2,
                                        "character": 19
                                    }
                                }
                            },
                            "message": "help: Use `local_name :: foreign_name (...)` with the real exported symbol, or move the declaration to the library that exports it."
                        }
                    ]
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
