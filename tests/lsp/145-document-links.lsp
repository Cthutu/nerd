use std.io
use test.parts
use test { grouped { alpha } }

main :: fn () {
    prn("answer {part_answer() + alpha()}")
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/documentLink",
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
            "diagnostics": [
                {
                    "range": {
                        "start": {
                            "line": 0,
                            "character": 4
                        },
                        "end": {
                            "line": 0,
                            "character": 10
                        }
                    },
                    "severity": 4,
                    "source": "nerd",
                    "message": "Unused use `std.io`",
                    "tags": [
                        1
                    ]
                },
                {
                    "range": {
                        "start": {
                            "line": 1,
                            "character": 4
                        },
                        "end": {
                            "line": 1,
                            "character": 14
                        }
                    },
                    "severity": 4,
                    "source": "nerd",
                    "message": "Unused use `test.parts`",
                    "tags": [
                        1
                    ]
                },
                {
                    "range": {
                        "start": {
                            "line": 2,
                            "character": 21
                        },
                        "end": {
                            "line": 2,
                            "character": 26
                        }
                    },
                    "severity": 4,
                    "source": "nerd",
                    "message": "Unused use `alpha`",
                    "tags": [
                        1
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
                "range": {
                    "start": {
                        "line": 0,
                        "character": 4
                    },
                    "end": {
                        "line": 0,
                        "character": 10
                    }
                },
                "target": "__REPO_URI__/mods/std/io.n"
            },
            {
                "range": {
                    "start": {
                        "line": 1,
                        "character": 4
                    },
                    "end": {
                        "line": 1,
                        "character": 14
                    }
                },
                "target": "__REPO_URI__/tests/mods/test/parts/mod.n"
            },
            {
                "range": {
                    "start": {
                        "line": 2,
                        "character": 21
                    },
                    "end": {
                        "line": 2,
                        "character": 26
                    }
                },
                "target": "__REPO_URI__/tests/mods/test/grouped/alpha.n"
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
