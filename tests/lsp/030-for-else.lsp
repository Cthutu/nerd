use std.io

main :: fn () {
    value :: for i := 0; i < 1; i += 1 {
        break i
    } else {
        break 99
    }
    prn($"{value}")
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
                        "."
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
                3,
                2,
                0,
                0,
                4,
                3,
                0,
                0,
                0,
                3,
                1,
                4,
                0,
                0,
                1,
                2,
                0,
                0,
                2,
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
                1,
                4,
                5,
                0,
                0,
                0,
                6,
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
                3,
                2,
                0,
                0,
                4,
                1,
                0,
                0,
                0,
                2,
                1,
                4,
                0,
                0,
                3,
                1,
                3,
                0,
                0,
                3,
                1,
                0,
                0,
                0,
                4,
                1,
                3,
                0,
                0,
                3,
                1,
                0,
                0,
                0,
                2,
                2,
                4,
                0,
                0,
                3,
                1,
                3,
                0,
                1,
                8,
                5,
                2,
                0,
                0,
                6,
                1,
                0,
                0,
                1,
                6,
                4,
                2,
                0,
                1,
                8,
                5,
                2,
                0,
                0,
                6,
                2,
                3,
                0,
                2,
                4,
                3,
                1,
                0,
                0,
                4,
                2,
                5,
                0,
                0,
                3,
                5,
                0,
                0,
                0,
                6,
                1,
                5,
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
