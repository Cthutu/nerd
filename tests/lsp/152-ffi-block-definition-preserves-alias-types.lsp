GLfloat :: f32

ffi "GL" {
    pub glClearColor      (red   : GLfloat,
                           green : GLfloat,
                           blue  : GLfloat,
                           alpha : GLfloat)
}

main :: fn () {
    glClearColor(0.0, 0.0, 0.0, 1.0)
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/hover",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 10,
                "character": 5
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "method": "textDocument/definition",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 10,
                "character": 5
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "method": "textDocument/signatureHelp",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 10,
                "character": 17
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
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nglClearColor :: fn (red: GLfloat, green: GLfloat, blue: GLfloat, alpha: GLfloat) -> void\n```\n\n- Kind: function"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "uri": "file:///test.n",
            "range": {
                "start": {
                    "line": 3,
                    "character": 8
                },
                "end": {
                    "line": 3,
                    "character": 20
                }
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": {
            "signatures": [
                {
                    "label": "glClearColor(red: GLfloat, green: GLfloat, blue: GLfloat, alpha: GLfloat) -> void",
                    "documentation": "Named arguments use `name = value`; omitted parameters use declared defaults when available.",
                    "parameters": [
                        {
                            "label": [
                                13,
                                25
                            ]
                        },
                        {
                            "label": [
                                27,
                                41
                            ]
                        },
                        {
                            "label": [
                                43,
                                56
                            ]
                        },
                        {
                            "label": [
                                58,
                                72
                            ]
                        }
                    ]
                }
            ],
            "activeSignature": 0,
            "activeParameter": 0
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
