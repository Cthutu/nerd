use std.frame
use std.gfx

consume :: fn (_value: u32) {
}

main :: fn () {
    gfx := GfxSystem.init()
    frame: Frame
    mode := PixelLayerMode.FixedSizeAutoScale { width: 4 height: 3 }
    handle := gfx.create_pixel_layer(^frame, mode)
    on gfx.get_pixel_layer(handle) => [p] {
        p.clear(0xff000000)
        width := p.pixel_width()
        consume(width)
    }
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/hover",
        "params": {
            "textDocument": { "uri": "file:///test.n" },
            "position": { "line": 11, "character": 39 }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "method": "textDocument/hover",
        "params": {
            "textDocument": { "uri": "file:///test.n" },
            "position": { "line": 12, "character": 8 }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "method": "textDocument/hover",
        "params": {
            "textDocument": { "uri": "file:///test.n" },
            "position": { "line": 12, "character": 11 }
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
                            "line": 14,
                            "character": 16
                        },
                        "end": {
                            "line": 14,
                            "character": 21
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Type mismatch: expected `u32`, found `u16`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 14,
                                        "character": 16
                                    },
                                    "end": {
                                        "line": 14,
                                        "character": 21
                                    }
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
        "id": 2,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\np\n```\n\n- Kind: pattern binder\n- Type: `^PixelLayer`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\np\n```\n\n- Kind: pattern binder\n- Type: `^PixelLayer`"
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nclear :: fn (layer: ^Self, colour: u32) -> void\n```\n\n- Kind: method\n\nClears every pixel to `colour`, which uses ARGB format (`0xAARRGGBB`)."
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
