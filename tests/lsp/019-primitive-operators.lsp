ratio: f64 = 1.5
main :: fn () => on (ratio >= 1.0 && !false) => 1 else 0
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
                "textDocumentSync": 1,
                "hoverProvider": true,
                "definitionProvider": true,
                "documentSymbolProvider": true,
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
                5,
                0,
                0,
                0,
                5,
                1,
                4,
                0,
                0,
                2,
                3,
                0,
                0,
                0,
                6,
                3,
                3,
                0,
                1,
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
                3,
                2,
                2,
                0,
                0,
                4,
                5,
                0,
                0,
                0,
                9,
                3,
                3,
                0,
                0,
                8,
                5,
                2,
                0,
                0,
                7,
                2,
                4,
                0,
                0,
                3,
                1,
                3,
                0,
                0,
                2,
                4,
                2,
                0,
                0,
                5,
                1,
                3,
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
