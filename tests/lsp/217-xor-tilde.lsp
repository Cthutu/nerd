main :: fn () {
    bits: u32 = 7
    bits ~= 3
    value := bits ~ ~1.as(u32)
    assert value == 0xfffffffa
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
                4,
                0,
                0,
                0,
                4,
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
                1,
                3,
                0,
                1,
                4,
                4,
                0,
                0,
                0,
                5,
                2,
                4,
                0,
                0,
                3,
                1,
                3,
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
                4,
                0,
                0,
                0,
                5,
                1,
                4,
                0,
                0,
                2,
                1,
                4,
                0,
                0,
                1,
                1,
                3,
                0,
                0,
                1,
                1,
                4,
                0,
                0,
                1,
                2,
                2,
                0,
                0,
                3,
                3,
                0,
                0,
                1,
                4,
                6,
                2,
                0,
                0,
                7,
                5,
                0,
                0,
                0,
                9,
                10,
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
