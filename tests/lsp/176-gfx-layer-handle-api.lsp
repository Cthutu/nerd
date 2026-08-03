use std.frame
use std.gfx

main :: fn () {
    gfx := GfxSystem.init()
    frame: Frame
    mode := PixelLayerMode.FixedSizeAutoScale { width: 4 height: 3 }
    handle := gfx.create_pixel_layer(^frame, mode)
    _ := gfx.get_pixel_layer(handle)
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 3,
        "method": "textDocument/hover",
        "params": {
            "textDocument": { "uri": "file:///test.n" },
            "position": { "line": 7, "character": 22 }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "method": "textDocument/definition",
        "params": {
            "textDocument": { "uri": "file:///test.n" },
            "position": { "line": 7, "character": 22 }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "method": "textDocument/hover",
        "params": {
            "textDocument": { "uri": "file:///test.n" },
            "position": { "line": 8, "character": 18 }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 6,
        "method": "textDocument/definition",
        "params": {
            "textDocument": { "uri": "file:///test.n" },
            "position": { "line": 8, "character": 18 }
        }
    }
]
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
                        "start": { "line": 0, "character": 4 },
                        "end": { "line": 0, "character": 13 }
                    },
                    "severity": 4,
                    "source": "nerd",
                    "message": "Unused use `std.frame`",
                    "tags": [1]
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\ncreate_pixel_layer :: fn (system: ^Self, frame: ^Frame, mode: PixelLayerMode) -> GfxLayerHandle\n```\n\n- Kind: method\n\nCreates a pixel layer for `frame` and returns its handle."
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": {
            "uri": "file:///home/matt/nerd/mods/std/gfx/mod.n",
            "range": {
                "start": { "line": 651, "character": 8 },
                "end": { "line": 651, "character": 26 }
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "result": {
            "contents": {
                "kind": "markdown",
                "value": "```nerd\nget_pixel_layer :: fn (system: ^Self, handle: GfxLayerHandle) -> ^PixelLayer\\GfxError\n```\n\n- Kind: method\n\nBorrows the pixel layer referenced by `handle` or returns a typed error."
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 6,
        "result": {
            "uri": "file:///home/matt/nerd/mods/std/gfx/mod.n",
            "range": {
                "start": { "line": 675, "character": 8 },
                "end": { "line": 675, "character": 23 }
            }
        }
    },
    { "jsonrpc": "2.0", "id": 999, "result": null }
]
