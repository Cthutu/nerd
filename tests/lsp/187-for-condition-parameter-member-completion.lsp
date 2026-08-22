use std.files
use std.process

main :: fn (args: []string) {
    i := 0
    for i < args. {
    }
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
                "line": 5,
                "character": 17
            },
            "context": {
                "triggerKind": 2,
                "triggerCharacter": "."
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
            "diagnostics": [
                {
                    "range": {
                        "start": {
                            "line": 5,
                            "character": 18
                        },
                        "end": {
                            "line": 5,
                            "character": 19
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Expected Symbol but found LeftBrace `{`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 5,
                                        "character": 18
                                    },
                                    "end": {
                                        "line": 5,
                                        "character": 19
                                    }
                                }
                            },
                            "message": "help: Check for a missing closing delimiter or misplaced operator"
                        }
                    ]
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 2,
        "result": [
            {
                "label": "data",
                "kind": 5,
                "detail": "field"
            },
            {
                "label": "count",
                "kind": 5,
                "detail": "field"
            },
            {
                "label": "size",
                "kind": 5,
                "detail": "field"
            },
            {
                "label": "bytes",
                "kind": 5,
                "detail": "field"
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
