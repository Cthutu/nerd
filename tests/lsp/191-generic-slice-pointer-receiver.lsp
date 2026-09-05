impl [T] []T
where T: Eq {
    contains :: fn (self: ^Self, value: T) -> bool {
        for i in [0 .. self.count] {
            on (self^)[i] == value => return true
        }
        return false
    }
}

main :: fn () -> i32 {
    return [1, 2, 3].contains(2).as(i32)
}
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
            "diagnostics": []
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
