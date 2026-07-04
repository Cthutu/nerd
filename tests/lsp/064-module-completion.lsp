use test.lsp_types

main :: fn () {}
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
                "line": 0,
                "character": 9
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
                            "character": 18
                        }
                    },
                    "severity": 4,
                    "source": "nerd",
                    "message": "Unused use `test.lsp_types`",
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
                "label": "associated_a",
                "kind": 9
            },
            {
                "label": "associated_b",
                "kind": 9
            },
            {
                "label": "associated_parts",
                "kind": 9
            },
            {
                "label": "broken_import_exports",
                "kind": 9
            },
            {
                "label": "completion_parts",
                "kind": 9
            },
            {
                "label": "embed_forms",
                "kind": 9
            },
            {
                "label": "embed_relative",
                "kind": 9
            },
            {
                "label": "ffi_alias",
                "kind": 9
            },
            {
                "label": "ffi_import",
                "kind": 9
            },
            {
                "label": "ffi_name_collision",
                "kind": 9
            },
            {
                "label": "ffi_name_collision_peer",
                "kind": 9
            },
            {
                "label": "folder_mod",
                "kind": 9
            },
            {
                "label": "folder_part_global",
                "kind": 9
            },
            {
                "label": "folder_priority",
                "kind": 9
            },
            {
                "label": "folder_pub_use",
                "kind": 9
            },
            {
                "label": "generics",
                "kind": 9
            },
            {
                "label": "imported_callback",
                "kind": 9
            },
            {
                "label": "imported_const_codegen",
                "kind": 9
            },
            {
                "label": "imported_const_plex",
                "kind": 9
            },
            {
                "label": "imported_plex",
                "kind": 9
            },
            {
                "label": "library_exports",
                "kind": 9
            },
            {
                "label": "local_bare_module",
                "kind": 9
            },
            {
                "label": "lsp_payloads",
                "kind": 9
            },
            {
                "label": "lsp_types",
                "kind": 9
            },
            {
                "label": "lsp_windows_exports",
                "kind": 9
            },
            {
                "label": "namespaced_a",
                "kind": 9
            },
            {
                "label": "namespaced_b",
                "kind": 9
            },
            {
                "label": "parts",
                "kind": 9
            },
            {
                "label": "platform_ffi",
                "kind": 9
            },
            {
                "label": "private_method",
                "kind": 9
            },
            {
                "label": "private_public_impl_import",
                "kind": 9
            },
            {
                "label": "private_public_method_import",
                "kind": 9
            },
            {
                "label": "public_impl",
                "kind": 9
            },
            {
                "label": "public_method",
                "kind": 9
            },
            {
                "label": "reexport",
                "kind": 9
            },
            {
                "label": "runtime_helper_u8",
                "kind": 9
            },
            {
                "label": "source_tests",
                "kind": 9
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
