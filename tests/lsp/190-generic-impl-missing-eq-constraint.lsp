impl [T] []T {
    contains :: fn (values: Self, expected: T) -> bool {
        return values[0] == expected
    }
}

main :: fn () => 0
¬
[]
¬
[
    {
        "jsonrpc": "2.0",
        "id": 1,
        "result": {
            "serverInfo": { "name": "Nerd LSP", "version": "0.1.0" },
            "capabilities": {
                "textDocumentSync": { "openClose": true, "change": 2 },
                "hoverProvider": true,
                "definitionProvider": true,
                "documentSymbolProvider": true,
                "completionProvider": {
                    "triggerCharacters": [".", "{"],
                    "resolveProvider": false
                },
                "signatureHelpProvider": {
                    "triggerCharacters": ["(", ","],
                    "retriggerCharacters": [",", "\n"]
                },
                "semanticTokensProvider": {
                    "legend": {
                        "tokenTypes": ["variable", "function", "keyword", "number", "operator", "string"],
                        "tokenModifiers": ["unnecessary"]
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
                        "start": { "line": 2, "character": 21 },
                        "end": { "line": 2, "character": 22 }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Type mismatch: expected `Eq constraint`, found `T`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": { "line": 2, "character": 21 },
                                    "end": { "line": 2, "character": 22 }
                                }
                            },
                            "message": "help: Change the expression or annotation so both sides use the same type."
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
