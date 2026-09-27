use std.vulkan
Event :: enum { Ready }
events :: fn (event: Event) {
    on event { Ready => {} }
}
main :: fn () {
    events(Ready)
    instance: VkInstance
    _result := vkCreateInstance(^{
        sType: VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO
        pApplicationInfo: ^{
            sType: VK_STRUCTURE_TYPE_APPLICATION_INFO
            pApplicationName: c"Example"
            apiVersion: VK_API_VERSION_1_4
            ...
        }
        ...
    }, nil, ^instance)
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
                "line": 16,
                "character": 8
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "method": "textDocument/completion",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 14,
                "character": 12
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "method": "textDocument/didChange",
        "params": {
            "textDocument": {
                "uri": "file:///test.n",
                "version": 2
            },
            "contentChanges": [
                {
                    "text": "use std.vulkan\nEvent :: enum { Ready }\nevents :: fn (event: Event) {\n    on event { Ready => {} }\n}\nmain :: fn () {\n    events(Ready)\n    instance: VkInstance\n    _result := vkCreateInstance(^{\n        sType: VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO\n        pApplicationInfo: ^{\n            sType: VK_STRUCTURE_TYPE_APPLICATION_INFO\n            pApplicationName: c\"Example\"\n            apiVersion: VK_API_VERSION_1_4\n            ...\n        }\n        p\n        ...\n    }, nil, ^instance)\n}\n"
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "method": "textDocument/completion",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 16,
                "character": 9
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "method": "textDocument/didChange",
        "params": {
            "textDocument": {
                "uri": "file:///test.n",
                "version": 3
            },
            "contentChanges": [
                {
                    "text": "use std.vulkan\nEvent :: enum { Ready }\nevents :: fn (event: Event) {\n    on event { Ready => {} }\n}\nmain :: fn () {\n    events(Ready)\n    instance: VkInstance\n    _result := vkCreateInstance(^{\n        sType: VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO\n        pApplicationInfo: ^{\n            sType: VK_STRUCTURE_TYPE_APPLICATION_INFO\n            pApplicationName: c\"Example\"\n            apiVersion: VK_API_VERSION_1_4\n            pE\n            ...\n        }\n        ...\n    }, nil, ^instance)\n}\n"
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "method": "textDocument/completion",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 14,
                "character": 14
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "method": "textDocument/didChange",
        "params": {
            "textDocument": {
                "uri": "file:///test.n",
                "version": 4
            },
            "contentChanges": [
                {
                    "text": "use std.vulkan\nEvent :: enum { Ready }\nevents :: fn (event: Event) {\n    on event { Ready => {} }\n}\nmain :: fn () {\n    events(Ready)\n    instance: VkInstance\n    _result := vkCreateInstance(^{\n        sType: VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO\n        pApplicationInfo: ^{\n            sType: VK_STRUCTURE_TYPE_APPLICATION_INFO\n            pApplicationName: c\"Example\"\n            apiVersion: VK_API_VERSION_1_4\n            ...\n        }\n        \n    }, nil, ^instance)\n}\n"
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 6,
        "method": "textDocument/completion",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 16,
                "character": 8
            }
        }
    },
    {
        "jsonrpc": "2.0",
        "method": "textDocument/didChange",
        "params": {
            "textDocument": {
                "uri": "file:///test.n",
                "version": 5
            },
            "contentChanges": [
                {
                    "text": "use std.vulkan\nEvent :: enum { Ready }\nevents :: fn (event: Event) {\n    on event { Ready => {} }\n}\nmain :: fn () {\n    events(Ready)\n    instance: VkInstance\n    _result := vkCreateInstance(^{\n        sType: VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO\n        pApplicationInfo: ^{\n            sType: VK_STRUCTURE_TYPE_APPLICATION_INFO\n            pApplicationName: c\"Example\"\n            apiVersion: VK_API_VERSION_1_4\n            ...\n        }\n        ...\n    }, nil, ^instance)\n}\n"
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 7,
        "method": "textDocument/completion",
        "params": {
            "textDocument": {
                "uri": "file:///test.n"
            },
            "position": {
                "line": 16,
                "character": 8
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
        "result": [
            {
                "label": "pNext",
                "kind": 5,
                "insertText": "pNext: "
            },
            {
                "label": "flags",
                "kind": 5,
                "insertText": "flags: "
            },
            {
                "label": "enabledLayerCount",
                "kind": 5,
                "insertText": "enabledLayerCount: "
            },
            {
                "label": "ppEnabledLayerNames",
                "kind": 5,
                "insertText": "ppEnabledLayerNames: "
            },
            {
                "label": "enabledExtensionCount",
                "kind": 5,
                "insertText": "enabledExtensionCount: "
            },
            {
                "label": "ppEnabledExtensionNames",
                "kind": 5,
                "insertText": "ppEnabledExtensionNames: "
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 3,
        "result": [
            {
                "label": "pNext",
                "kind": 5,
                "insertText": "pNext: "
            },
            {
                "label": "applicationVersion",
                "kind": 5,
                "insertText": "applicationVersion: "
            },
            {
                "label": "pEngineName",
                "kind": 5,
                "insertText": "pEngineName: "
            },
            {
                "label": "engineVersion",
                "kind": 5,
                "insertText": "engineVersion: "
            }
        ]
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
                            "line": 16,
                            "character": 8
                        },
                        "end": {
                            "line": 16,
                            "character": 9
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Unknown field `p` in plex literal of type `VkInstanceCreateInfo`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "__REPO_URI__/mods/std/vulkan/mod.n",
                                "range": {
                                    "start": {
                                        "line": 212,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 212,
                                        "character": 24
                                    }
                                }
                            },
                            "message": "Type `VkInstanceCreateInfo` is defined here"
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 16,
                                        "character": 8
                                    },
                                    "end": {
                                        "line": 16,
                                        "character": 9
                                    }
                                }
                            },
                            "message": "help: Use a field declared by the target plex type."
                        }
                    ]
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 4,
        "result": [
            {
                "label": "pNext",
                "kind": 5,
                "insertText": "pNext: "
            },
            {
                "label": "ppEnabledLayerNames",
                "kind": 5,
                "insertText": "ppEnabledLayerNames: "
            },
            {
                "label": "ppEnabledExtensionNames",
                "kind": 5,
                "insertText": "ppEnabledExtensionNames: "
            }
        ]
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
                            "character": 12
                        },
                        "end": {
                            "line": 14,
                            "character": 14
                        }
                    },
                    "severity": 1,
                    "source": "nerd",
                    "message": "Unknown field `pE` in plex literal of type `VkApplicationInfo`",
                    "relatedInformation": [
                        {
                            "location": {
                                "uri": "__REPO_URI__/mods/std/vulkan/mod.n",
                                "range": {
                                    "start": {
                                        "line": 192,
                                        "character": 4
                                    },
                                    "end": {
                                        "line": 192,
                                        "character": 21
                                    }
                                }
                            },
                            "message": "Type `VkApplicationInfo` is defined here"
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 14,
                                        "character": 12
                                    },
                                    "end": {
                                        "line": 14,
                                        "character": 14
                                    }
                                }
                            },
                            "message": "help: Use a field declared by the target plex type."
                        }
                    ]
                }
            ]
        }
    },
    {
        "jsonrpc": "2.0",
        "id": 5,
        "result": [
            {
                "label": "pEngineName",
                "kind": 5,
                "insertText": "pEngineName: "
            }
        ]
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
                            "line": 8,
                            "character": 33
                        },
                        "end": {
                            "line": 8,
                            "character": 34
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
                                        "line": 8,
                                        "character": 33
                                    },
                                    "end": {
                                        "line": 8,
                                        "character": 34
                                    }
                                }
                            },
                            "message": "note: Missing fields: `pNext`, `flags`, `enabledLayerCount`, `ppEnabledLayerNames`, `enabledExtensionCount`, `ppEnabledExtensionNames`"
                        },
                        {
                            "location": {
                                "uri": "file:///test.n",
                                "range": {
                                    "start": {
                                        "line": 8,
                                        "character": 33
                                    },
                                    "end": {
                                        "line": 8,
                                        "character": 34
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
        "id": 6,
        "result": [
            {
                "label": "pNext",
                "kind": 5,
                "insertText": "pNext: "
            },
            {
                "label": "flags",
                "kind": 5,
                "insertText": "flags: "
            },
            {
                "label": "enabledLayerCount",
                "kind": 5,
                "insertText": "enabledLayerCount: "
            },
            {
                "label": "ppEnabledLayerNames",
                "kind": 5,
                "insertText": "ppEnabledLayerNames: "
            },
            {
                "label": "enabledExtensionCount",
                "kind": 5,
                "insertText": "enabledExtensionCount: "
            },
            {
                "label": "ppEnabledExtensionNames",
                "kind": 5,
                "insertText": "ppEnabledExtensionNames: "
            }
        ]
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
        "id": 7,
        "result": [
            {
                "label": "pNext",
                "kind": 5,
                "insertText": "pNext: "
            },
            {
                "label": "flags",
                "kind": 5,
                "insertText": "flags: "
            },
            {
                "label": "enabledLayerCount",
                "kind": 5,
                "insertText": "enabledLayerCount: "
            },
            {
                "label": "ppEnabledLayerNames",
                "kind": 5,
                "insertText": "ppEnabledLayerNames: "
            },
            {
                "label": "enabledExtensionCount",
                "kind": 5,
                "insertText": "enabledExtensionCount: "
            },
            {
                "label": "ppEnabledExtensionNames",
                "kind": 5,
                "insertText": "ppEnabledExtensionNames: "
            }
        ]
    },
    {
        "jsonrpc": "2.0",
        "id": 999,
        "result": null
    }
]
