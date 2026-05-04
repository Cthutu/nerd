value :: 1

main :: fn () {
    local := value
    local
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/completion",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 4,
                "character": 5
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
        "result": [
            {
                "label": "assert",
                "kind": 14
            },
            {
                "label": "break",
                "kind": 14
            },
            {
                "label": "continue",
                "kind": 14
            },
            {
                "label": "defer",
                "kind": 14
            },
            {
                "label": "else",
                "kind": 14
            },
            {
                "label": "enum",
                "kind": 14
            },
            {
                "label": "ffi",
                "kind": 14
            },
            {
                "label": "fn",
                "kind": 14
            },
            {
                "label": "for",
                "kind": 14
            },
            {
                "label": "impl",
                "kind": 14
            },
            {
                "label": "on",
                "kind": 14
            },
            {
                "label": "pub",
                "kind": 14
            },
            {
                "label": "return",
                "kind": 14
            },
            {
                "label": "test",
                "kind": 14
            },
            {
                "label": "union",
                "kind": 14
            },
            {
                "label": "use",
                "kind": 14
            },
            {
                "label": "value",
                "kind": 21
            },
            {
                "label": "main",
                "kind": 3
            },
            {
                "label": "local",
                "kind": 6
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
