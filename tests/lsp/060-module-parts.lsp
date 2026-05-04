main :: fn () => 0
¬
[
    {
        "jsonrpc": "2.0",
        "method": "textDocument/didOpen",
        "params": {
            "textDocument": {
                "uri": "file:///home/matt/nerd/tests/mods/test/parts/body.n",
                "languageId": "nerd",
                "version": 1,
                "text": "pub make_thing :: fn (value: i32) -> Thing {\n    return Thing { value: value }\n}\n\npub part_answer :: fn () -> i32 {\n    thing := make_thing(42)\n    return thing.value\n}\n"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/semanticTokens/full",
        "params": {
            "textDocument": {
                "uri": "file:///home/matt/nerd/tests/mods/test/parts/body.n"
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
        "method": "textDocument/publishDiagnostics",
        "params": {
            "uri": "__REPO_URI__/tests/mods/test/parts/body.n",
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
                10,
                1,
                0,
                0,
                11,
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
                5,
                2,
                4,
                0,
                0,
                3,
                5,
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
                8,
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
                5,
                0,
                0,
                3,
                0,
                3,
                2,
                0,
                0,
                4,
                11,
                1,
                0,
                0,
                12,
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
                3,
                0,
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
                10,
                1,
                0,
                0,
                11,
                2,
                3,
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
                5,
                1,
                4,
                0,
                0,
                1,
                5,
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
