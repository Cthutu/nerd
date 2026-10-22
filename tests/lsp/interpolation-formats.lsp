main :: fn () {
    value: u32 = 10
    text := $"{value:02X}/{value:08b}"
    assert text.count == 11
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
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 2,
                "character": 15
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
                2,
                3,
                0,
                1,
                4,
                4,
                0,
                0,
                0,
                5,
                1,
                4,
                0,
                0,
                3,
                2,
                5,
                0,
                0,
                3,
                5,
                0,
                0,
                0,
                5,
                4,
                5,
                0,
                0,
                5,
                1,
                5,
                0,
                0,
                2,
                5,
                0,
                0,
                0,
                5,
                4,
                5,
                0,
                0,
                5,
                1,
                5,
                0,
                1,
                4,
                6,
                2,
                0,
                0,
                7,
                4,
                0,
                0,
                0,
                4,
                1,
                4,
                0,
                0,
                1,
                5,
                0,
                0,
                0,
                9,
                2,
                3,
                0
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nvalue\n```\n\n- Kind: local variable\n- Type: `u32`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
