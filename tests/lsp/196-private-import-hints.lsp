use private_hint_a
use private_hint_b
main :: fn () { _value: VertexArray }
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
                            "line": 2,
                            "character": 24
                        },
                        "end": {
                            "line": 2,
                            "character": 35
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Unknown type `VertexArray`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 2,
                                        "character": 24
                                    },
                                    "end": {
                                        "line": 2,
                                        "character": 35
                                    }
                                }
                            },
                            "message": "help: Use a defined type name, or one of the built-in primitive types."
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 2,
                                        "character": 24
                                    },
                                    "end": {
                                        "line": 2,
                                        "character": 35
                                    }
                                }
                            },
                            "message": "help: Used module `private_hint_a` declares `VertexArray` privately. Did you intend to mark that declaration `pub`?"
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 2,
                                        "character": 24
                                    },
                                    "end": {
                                        "line": 2,
                                        "character": 35
                                    }
                                }
                            },
                            "message": "help: Used module `private_hint_b` declares `VertexArray` privately. Did you intend to mark that declaration `pub`?"
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
