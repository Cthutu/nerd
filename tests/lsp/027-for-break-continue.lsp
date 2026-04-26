main :: fn () {
    total := 0
    for i := 0; i < 6; i += 1 {
        on i == 2 => continue
        on i == 5 => break
        total += i
    }
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
                3,
                1,
                3,
                0,
                1,
                4,
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
                2,
                2,
                0,
                0,
                3,
                1,
                0,
                0,
                0,
                5,
                1,
                3,
                0,
                0,
                2,
                2,
                4,
                0,
                0,
                3,
                8,
                2,
                0,
                1,
                8,
                2,
                2,
                0,
                0,
                3,
                1,
                0,
                0,
                0,
                5,
                1,
                3,
                0,
                0,
                2,
                2,
                4,
                0,
                0,
                3,
                5,
                2,
                0,
                1,
                8,
                5,
                0,
                0,
                0,
                6,
                2,
                4,
                0,
                0,
                3,
                1,
                0,
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
