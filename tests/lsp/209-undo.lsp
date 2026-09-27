acquire :: fn () -> ?void {
}

main :: fn () -> ?void {
    acquire()?
    undo prn("first")
    undo {
        prn("second")
    }
    return
}

¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/semanticTokens/full",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
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
            "data": [
                0,
                0,
                7,
                1,
                0,
                0,
                8,
                1,
                4,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                2,
                2,
                2,
                0,
                0,
                6,
                2,
                4,
                0,
                0,
                4,
                4,
                0,
                0,
                3,
                0,
                4,
                1,
                0,
                0,
                5,
                1,
                4,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                2,
                2,
                2,
                0,
                0,
                6,
                2,
                4,
                0,
                0,
                4,
                4,
                0,
                0,
                1,
                4,
                7,
                1,
                0,
                1,
                4,
                4,
                2,
                0,
                0,
                5,
                3,
                1,
                0,
                0,
                4,
                7,
                5,
                0,
                1,
                4,
                4,
                2,
                0,
                1,
                8,
                3,
                1,
                0,
                0,
                4,
                8,
                5,
                0,
                2,
                4,
                6,
                2,
                0
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
