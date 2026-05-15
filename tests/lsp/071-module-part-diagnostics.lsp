main :: fn () => 0
¬
[
    {
        "jsonrpc": "2.0",
        "method": "textDocument/didOpen",
        "params": {
            "textDocument": {
                "uri": "__REPO_URI__/tests/mods/test/parts/body.n",
                "languageId": "nerd",
                "version": 1,
                "text": "pub make_thing :: fn (value: MissingType) -> Thing {\n    return Thing { value: value }\n}\n\npub part_answer :: fn () -> i32 {\n    return 42\n}\n"
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
            "diagnostics": []
        }
    },
    {
        "jsonrpc": "2.0",
        "method": "textDocument/publishDiagnostics",
        "params": {
            "uri": "__REPO_URI__/tests/mods/test/parts/body.n",
            "diagnostics": [
                {
                    "range": {
                        "start": {
                            "line": 0,
                            "character": 29
                        },
                        "end": {
                            "line": 0,
                            "character": 40
                        }
                    },
                    "severity": 1,
                    "code": "0303",
                    "source": "nerd",
                    "message": "Unknown type `MissingType`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "__REPO_URI__/tests/mods/test/parts/body.n",
                                "range": {
                                    "start": {
                                        "line": 0,
                                        "character": 29
                                    },
                                    "end": {
                                        "line": 0,
                                        "character": 40
                                    }
                                }
                            },
                            "message": "help: Use a defined type name, or one of the built-in primitive types."
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
