use std.vulkan

main :: fn () {
    _info := VkInstanceCreateInfo {
        sType: VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO
        pNext: ^VkDebugUtilsMessengerCreateInfoEXT {
        }
        ...
    }
}
¬
[
    {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "textDocument/codeAction",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "range": {
                "start": {
                    "line": 5,
                    "character": 52
                },
                "end": {
                    "line": 5,
                    "character": 52
                }
            },
            "context": {
                "diagnostics": []
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
                            "character": 51
                        },
                        "end": {
                            "line": 5,
                            "character": 52
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Plex literal is missing required fields",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 5,
                                        "character": 51
                                    },
                                    "end": {
                                        "line": 5,
                                        "character": 52
                                    }
                                }
                            },
                            "message": "note: Missing fields: `sType`, `pNext`, `flags`, `messageSeverity`, `messageType`, `pfnUserCallback`, `pUserData`"
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 5,
                                        "character": 51
                                    },
                                    "end": {
                                        "line": 5,
                                        "character": 52
                                    }
                                }
                            },
                            "message": "help: Add all fields required by the plex type, or write `...` in the literal to initialise omitted fields with their default values."
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
                "title": "Fill missing plex fields",
                "kind": "quickfix",
                "edit": {
                    "changes": {
                        "file:///test.n": [
                            {
                                "range": {
                                    "start": {
                                        "line": 5,
                                        "character": 52
                                    },
                                    "end": {
                                        "line": 6,
                                        "character": 8
                                    }
                                },
                                "newText": "\n            sType          : 0\n            pNext          : nil\n            flags          : 0\n            messageSeverity: 0\n            messageType    : 0\n            pfnUserCallback: nil\n            pUserData      : nil\n        "
                            }
                        ]
                    }
                }
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
