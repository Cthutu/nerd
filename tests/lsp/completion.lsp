main :: fn () {
    items: [4..]string = ["north", "south"]
    items.push("east")
    _cap := items.capacity
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
        "line": 1,
        "character": 4
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 3,
        "character": 18
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 3,
        "character": 12
      }
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
        "line": 2,
        "character": 10
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
        "value": "```nerd\nitems\n```\n\n- Kind: local variable\n- Type: `[..]string`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\ncapacity\n```\n\n- Kind: dynamic array field\n- Type: `usize`\n- Owner: `[..]string`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "uri": "file:///test.n",
      "range": {
        "start": {
          "line": 1,
          "character": 4
        },
        "end": {
          "line": 1,
          "character": 9
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
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
        "label": "capacity",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "append",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "clear",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "delete",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "free",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "pop",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "push",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "reserve_to",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "reserve_extra",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "resize_to",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "swap_delete",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "resize_undefined_to",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "extend",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "extend_undefined",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "size",
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

¬
value :: 1

main :: fn () {
    local := value
    _ := local
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
        "line": 4,
        "character": 10
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
        "label": "local",
        "kind": 6
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
Point :: plex {
    x i32
    y i32
}

Counter :: plex {
    use Point
    value i32
}

impl Counter {
    inc :: fn (self: ^Self, amount: i32) {
        self.value += amount
    }

    get :: fn (self: Self) -> i32 {
        return self.value
    }
}

main :: fn () {
    counter: Counter
    _ := counter.x
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
        "line": 22,
        "character": 17
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 22,
        "character": 17
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 22,
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
    "result": [
      {
        "label": "x",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "y",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "value",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "inc",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "get",
        "kind": 2,
        "detail": "method"
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nx\n```\n\n- Kind: plex field\n- Type: `i32`\n- Owner: `Counter`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "uri": "file:///test.n",
      "range": {
        "start": {
          "line": 1,
          "character": 4
        },
        "end": {
          "line": 1,
          "character": 5
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
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
        "label": "anonymous_records",
        "kind": 9
      },
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
        "label": "compiler_constructs",
        "kind": 9
      },
      {
        "label": "completion_parts",
        "kind": 9
      },
      {
        "label": "compound",
        "kind": 9
      },
      {
        "label": "default_arena",
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
        "label": "file_module",
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
        "label": "named_defaults",
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
        "label": "optional_defaults",
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
        "label": "qualified_generics",
        "kind": 9
      },
      {
        "label": "recursive_message_owner",
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

¬
io :: use std.io

main :: fn () {
    line := io.input("name: ")
    prn(line.as(string))
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
        "line": 3,
        "character": 15
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
        "label": "input",
        "kind": 3
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
Counter :: plex {
    value i32
}

counter: Counter

main :: fn () {
    _ := counter.value
    missing_symbol()
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
        "line": 7,
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
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 8,
              "character": 4
            },
            "end": {
              "line": 8,
              "character": 18
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown symbol `missing_symbol`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 8,
                    "character": 4
                  },
                  "end": {
                    "line": 8,
                    "character": 18
                  }
                }
              },
              "message": "help: Add a binding for `missing_symbol` or fix the spelling."
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
        "label": "value",
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

¬
Frame :: plex {
    id i32
}

frame :: 1
Frob :: 2

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
        "character": 2
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
        "label": "Frame",
        "kind": 22
      },
      {
        "label": "Frob",
        "kind": 21
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
boxmod :: use test.imported_plex

main :: fn () {
    missing()
    boxmod.
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
        "line": 4,
        "character": 11
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
              "character": 0
            },
            "end": {
              "line": 5,
              "character": 1
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 5,
                    "character": 0
                  },
                  "end": {
                    "line": 5,
                    "character": 1
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
        "label": "Box",
        "kind": 22
      },
      {
        "label": "make_box",
        "kind": 3
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
broken :: use test.broken_import_exports

main :: fn () {
    broken.
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
        "line": 3,
        "character": 11
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
              "line": 4,
              "character": 0
            },
            "end": {
              "line": 4,
              "character": 1
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 4,
                    "character": 0
                  },
                  "end": {
                    "line": 4,
                    "character": 1
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
        "label": "Box",
        "kind": 22
      },
      {
        "label": "make_box",
        "kind": 3
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
broken :: use test.broken_import_exports

main :: fn () {
    _ := broken.make_box
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
        "line": 3,
        "character": 16
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
    "method": "textDocument/publishDiagnostics",
    "params": {
      "uri": "__REPO_URI__/tests/mods/test/broken_import_exports.n",
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 0,
              "character": 4
            },
            "end": {
              "line": 0,
              "character": 11
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Type mismatch: expected `known module`, found `module path`",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/tests/mods/test/broken_import_exports.n",
                "range": {
                  "start": {
                    "line": 0,
                    "character": 4
                  },
                  "end": {
                    "line": 0,
                    "character": 11
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
    "result": [
      {
        "label": "Box",
        "kind": 22
      },
      {
        "label": "make_box",
        "kind": 3
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
main :: fn () {}
¬
[
  {
    "jsonrpc": "2.0",
    "method": "textDocument/didOpen",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/078-current-module-completion/main.n",
        "languageId": "nerd",
        "version": 1,
        "text": "x11 :: use x11\n\nmain :: fn () {\n        \n}\n"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "method": "textDocument/didChange",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/078-current-module-completion/main.n",
        "version": 2
      },
      "contentChanges": [
        {
          "range": {
            "start": {
              "line": 3,
              "character": 8
            },
            "end": {
              "line": 3,
              "character": 8
            }
          },
          "text": "display := x11"
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "method": "textDocument/didChange",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/078-current-module-completion/main.n",
        "version": 3
      },
      "contentChanges": [
        {
          "range": {
            "start": {
              "line": 3,
              "character": 22
            },
            "end": {
              "line": 3,
              "character": 22
            }
          },
          "text": "."
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/completion",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/078-current-module-completion/main.n"
      },
      "position": {
        "line": 3,
        "character": 23
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
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "method": "textDocument/publishDiagnostics",
    "params": {
      "uri": "__REPO_URI__/tests/lsp/078-current-module-completion/main.n",
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "method": "textDocument/publishDiagnostics",
    "params": {
      "uri": "__REPO_URI__/tests/lsp/078-current-module-completion/main.n",
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 3,
              "character": 8
            },
            "end": {
              "line": 3,
              "character": 15
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Invalid variable type `module`",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/tests/lsp/078-current-module-completion/main.n",
                "range": {
                  "start": {
                    "line": 3,
                    "character": 8
                  },
                  "end": {
                    "line": 3,
                    "character": 15
                  }
                }
              },
              "message": "help: Variables may use concrete integer, `bool`, `string`, `f32`, or `f64` types."
            }
          ]
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "method": "textDocument/publishDiagnostics",
    "params": {
      "uri": "__REPO_URI__/tests/lsp/078-current-module-completion/main.n",
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 4,
              "character": 0
            },
            "end": {
              "line": 4,
              "character": 1
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/tests/lsp/078-current-module-completion/main.n",
                "range": {
                  "start": {
                    "line": 4,
                    "character": 0
                  },
                  "end": {
                    "line": 4,
                    "character": 1
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
        "label": "Display",
        "kind": 22
      },
      {
        "label": "Window",
        "kind": 22
      },
      {
        "label": "XOpenDisplay",
        "kind": 3
      },
      {
        "label": "XCloseDisplay",
        "kind": 3
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
parts :: use test.completion_parts

main :: fn () {
    parts.
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
        "line": 3,
        "character": 10
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
              "line": 4,
              "character": 0
            },
            "end": {
              "line": 4,
              "character": 1
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 4,
                    "character": 0
                  },
                  "end": {
                    "line": 4,
                    "character": 1
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
        "label": "RootValue",
        "kind": 22
      },
      {
        "label": "PartValue",
        "kind": 22
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
KeyEvent :: plex {
    code i32
    mods i32
}

Event :: enum {
    KeyDown(KeyEvent)
}

main :: fn () {
    event : Event
    on event {
        KeyDown(key_event) => {
            key_event.
        }
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
        "line": 13,
        "character": 22
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
              "line": 14,
              "character": 8
            },
            "end": {
              "line": 14,
              "character": 9
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 14,
                    "character": 8
                  },
                  "end": {
                    "line": 14,
                    "character": 9
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
        "label": "code",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "mods",
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

¬
payloads :: use test.lsp_payloads

main :: fn () {
    event : payloads.Event
    on event {
        KeyDown(key_event) => {
            key_event.
        }
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
        "line": 6,
        "character": 22
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
              "line": 7,
              "character": 8
            },
            "end": {
              "line": 7,
              "character": 9
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 7,
                    "character": 8
                  },
                  "end": {
                    "line": 7,
                    "character": 9
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
        "label": "keycode",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "modifiers",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "char",
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

¬
Frame :: plex {
    handle u64
    system ^FrameSystem
}

FrameSystem :: plex {
    frames [..]Frame
}

impl FrameSystem {
    done :: fn (frame: ^Frame) {
        frame.
    }
}

broken: fn ()
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
        "line": 11,
        "character": 14
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
              "line": 12,
              "character": 4
            },
            "end": {
              "line": 12,
              "character": 5
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 12,
                    "character": 4
                  },
                  "end": {
                    "line": 12,
                    "character": 5
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
        "label": "handle",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "system",
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

¬
Thing :: plex {
    value i32
}

answer :: 42

main :: fn () {
    Thi
}

broken : fn(frame: ^Thing) {
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
        "line": 7,
        "character": 7
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
              "line": 10,
              "character": 9
            },
            "end": {
              "line": 10,
              "character": 11
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Colon `:` but found Keyword `fn`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 10,
                    "character": 9
                  },
                  "end": {
                    "line": 10,
                    "character": 11
                  }
                }
              },
              "message": "note: Function values use a binding operator before `fn`, not a type annotation colon."
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 10,
                    "character": 9
                  },
                  "end": {
                    "line": 10,
                    "character": 11
                  }
                }
              },
              "message": "help: Use `:: fn (...) { ... }` for a constant function binding, or `:= fn (...) { ... }` for a mutable function value."
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
        "label": "Thing",
        "kind": 1
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
Frame :: plex {
    handle u64
    system ^FrameSystem
}

FrameInfo :: plex {
    handle u64
    frame ^Frame
}

FrameSystem :: plex {
    frames [..]FrameInfo
}

impl Frame {
    done :: fn (frame: ^Self) {
        frame.handle = 0
    }
}

impl FrameSystem {
    find :: fn (fs: ^Self) {
        for frame in fs.frames {
            frame.
        }
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
        "line": 23,
        "character": 18
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
              "line": 24,
              "character": 8
            },
            "end": {
              "line": 24,
              "character": 9
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 24,
                    "character": 8
                  },
                  "end": {
                    "line": 24,
                    "character": 9
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
        "label": "handle",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "frame",
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

¬
Frame :: plex {
    handle u64
    system ^FrameSystem
}

FrameInfo :: plex {
    handle u64
    frame ^Frame
}

FrameSystem :: plex {
    frames [..]FrameInfo
}

impl Frame {
    done :: fn (frame: ^Self) {
        frame.handle = 0
    }
}

impl FrameSystem {
    find :: fn (fs: ^Self) {
        return for frame in fs.frames {
            frame.
        }
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
        "line": 23,
        "character": 18
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
              "line": 24,
              "character": 8
            },
            "end": {
              "line": 24,
              "character": 9
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 24,
                    "character": 8
                  },
                  "end": {
                    "line": 24,
                    "character": 9
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
        "label": "handle",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "frame",
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

¬
FrameInfo :: plex {
    handle u64
}

FrameSystem :: plex {
    frames [..]FrameInfo
}

impl FrameSystem {
    done :: fn (fs: ^Self) {
        fs.frames.
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
        "line": 10,
        "character": 18
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
              "line": 11,
              "character": 4
            },
            "end": {
              "line": 11,
              "character": 5
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 11,
                    "character": 4
                  },
                  "end": {
                    "line": 11,
                    "character": 5
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
        "label": "capacity",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "append",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "clear",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "delete",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "free",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "pop",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "push",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "reserve_to",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "reserve_extra",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "resize_to",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "swap_delete",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "resize_undefined_to",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "extend",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "extend_undefined",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "size",
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

¬
-- Core bindings are implicit.

Owner :: plex {
    scratch arena
    broken : string -- Keep completion available after an earlier error.
}

main :: fn () {
    arena_var := arena(16, 8)
    _ := arena_var.

    text := "hello"
    _ := text.
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
        "line": 9,
        "character": 19
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
        "line": 12,
        "character": 14
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
              "line": 4,
              "character": 11
            },
            "end": {
              "line": 4,
              "character": 12
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected type but found Colon `:`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 4,
                    "character": 11
                  },
                  "end": {
                    "line": 4,
                    "character": 12
                  }
                }
              },
              "message": "note: Plex field definitions are written as `field Type`."
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 4,
                    "character": 11
                  },
                  "end": {
                    "line": 4,
                    "character": 12
                  }
                }
              },
              "message": "help: Remove the colon. Colons are used in plex literals such as `State { loc_index: 0 }`, not in plex definitions."
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
        "label": "pr",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "prn",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "alloc",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "alloc_array",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "alloc_bytes",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "reset",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "mark",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "restore",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "done",
        "kind": 2,
        "detail": "method"
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
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

¬
FrameInfo :: plex {
    handle u64
    title_heap []
}

FrameSystem :: plex {
    frames [..]FrameInfo
}

impl FrameSystem {
    delete_frame_info :: fn (fs: ^Self, handle: u64) {
        for i, frame in fs.frames {
            on frame.handle == handle => {
                frame.title_heap.
            }
        }
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
        "line": 13,
        "character": 33
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
              "line": 3,
              "character": 0
            },
            "end": {
              "line": 3,
              "character": 1
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected declaration or expression but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 3,
                    "character": 0
                  },
                  "end": {
                    "line": 3,
                    "character": 1
                  }
                }
              },
              "message": "help: Expected a type annotation after ':', but found RightBrace `}`"
            }
          ]
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": []
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
Frame :: plex {
    handle u64
    system ^FrameSystem
}

FrameSystem :: plex {
    frames [..]Frame
}

done :: fn (frame: ^Frame) {
    _ := frame.handle
}
¬
[
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
          "range": {
            "start": {
              "line": 10,
              "character": 15
            },
            "end": {
              "line": 10,
              "character": 21
            }
          },
          "text": ""
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/completion",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 10,
        "character": 15
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
      "diagnostics": []
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
              "line": 11,
              "character": 0
            },
            "end": {
              "line": 11,
              "character": 1
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 11,
                    "character": 0
                  },
                  "end": {
                    "line": 11,
                    "character": 1
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
        "label": "handle",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "system",
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

¬
Thing :: plex {
    value i32
}

broken : fn(frame: ^Thing) {
    fra
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
        "character": 7
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
              "line": 4,
              "character": 9
            },
            "end": {
              "line": 4,
              "character": 11
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Colon `:` but found Keyword `fn`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 4,
                    "character": 9
                  },
                  "end": {
                    "line": 4,
                    "character": 11
                  }
                }
              },
              "message": "note: Function values use a binding operator before `fn`, not a type annotation colon."
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 4,
                    "character": 9
                  },
                  "end": {
                    "line": 4,
                    "character": 11
                  }
                }
              },
              "message": "help: Use `:: fn (...) { ... }` for a constant function binding, or `:= fn (...) { ... }` for a mutable function value."
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
        "label": "frame",
        "kind": 6
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
value :: 1
other :: 2
main :: fn () {
    result := add(val
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
        "line": 3,
        "character": 21
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
              "line": 4,
              "character": 0
            },
            "end": {
              "line": 4,
              "character": 1
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected RightParen `)` but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 4,
                    "character": 0
                  },
                  "end": {
                    "line": 4,
                    "character": 1
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
        "label": "value",
        "kind": 1
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
main :: fn () {
    pair: (i32, string) = (1, "one")
    pair_ptr := ^pair
    values: [3]i32 = [1, 2, 3]
    slice := values[..]
    slice_ptr := ^slice
    _ := pair_ptr.0
    _ := values.count
    _ := slice_ptr.count
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
        "line": 6,
        "character": 18
      },
      "context": {
        "triggerKind": 2,
        "triggerCharacter": "."
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
        "line": 7,
        "character": 16
      },
      "context": {
        "triggerKind": 2,
        "triggerCharacter": "."
      }
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
        "line": 8,
        "character": 19
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
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": [
      {
        "label": "0",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "1",
        "kind": 5,
        "detail": "field"
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
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
    "id": 4,
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

¬
use test.lsp_types
pragmawindowed

main :: fn () {
    Po
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
        "line": 4,
        "character": 6
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
              "line": 3,
              "character": 0
            },
            "end": {
              "line": 3,
              "character": 4
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unexpected Symbol after expression",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 3,
                    "character": 0
                  },
                  "end": {
                    "line": 3,
                    "character": 4
                  }
                }
              },
              "message": "help: Expected a symbol to start a binding"
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
        "label": "Point",
        "kind": 22
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
use win

Point :: plex {
    x i32
    y i32
    visible bool
}

main :: fn () {
    point := Point {
        x: 1
        v
        q
    }
    class := WNDCLASSA {
        style: 0
        l
    }
}
¬
[
  {
    "jsonrpc": "2.0",
    "method": "textDocument/didOpen",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/112-plex-literal-field-completion/main.n",
        "languageId": "nerd",
        "version": 1,
        "text": "win :: use win\nuse win\n\nPoint :: plex {\n    x i32\n    y i32\n    visible bool\n}\n\nmain :: fn () {\n    point := Point {\n        x: 1\n        v\n        q\n    }\n    class := win.WNDCLASSA {\n        style: 0\n        lp\n    }\n    unqualified := WNDCLASSA {\n        style: 0\n        l\n    }\n}\n"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/completion",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/112-plex-literal-field-completion/main.n"
      },
      "position": {
        "line": 11,
        "character": 9
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/completion",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/112-plex-literal-field-completion/main.n"
      },
      "position": {
        "line": 16,
        "character": 10
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "method": "textDocument/completion",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/112-plex-literal-field-completion/main.n"
      },
      "position": {
        "line": 12,
        "character": 9
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "method": "textDocument/completion",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/112-plex-literal-field-completion/main.n"
      },
      "position": {
        "line": 20,
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
              "character": 7
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Type mismatch: expected `known module`, found `module path`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 0,
                    "character": 4
                  },
                  "end": {
                    "line": 0,
                    "character": 7
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
    "method": "textDocument/publishDiagnostics",
    "params": {
      "uri": "__REPO_URI__/tests/lsp/112-plex-literal-field-completion/main.n",
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 12,
              "character": 8
            },
            "end": {
              "line": 12,
              "character": 9
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown field `v` in plex literal of type `Point`",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/tests/lsp/112-plex-literal-field-completion/main.n",
                "range": {
                  "start": {
                    "line": 3,
                    "character": 0
                  },
                  "end": {
                    "line": 3,
                    "character": 5
                  }
                }
              },
              "message": "Type `Point` is defined here"
            },
            {
              "location": {
                "uri": "__REPO_URI__/tests/lsp/112-plex-literal-field-completion/main.n",
                "range": {
                  "start": {
                    "line": 12,
                    "character": 8
                  },
                  "end": {
                    "line": 12,
                    "character": 9
                  }
                }
              },
              "message": "help: Did you mean `x`?"
            }
          ]
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": []
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": []
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": [
      {
        "label": "visible",
        "kind": 5,
        "insertText": "visible: "
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "result": []
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
use os.
use os.windows.

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
        "character": 7
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
        "line": 1,
        "character": 15
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
              "line": 1,
              "character": 0
            },
            "end": {
              "line": 1,
              "character": 3
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found Keyword `use`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 1,
                    "character": 0
                  },
                  "end": {
                    "line": 1,
                    "character": 3
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
        "label": "linux",
        "kind": 9
      },
      {
        "label": "socket",
        "kind": 9
      },
      {
        "label": "thread",
        "kind": 9
      },
      {
        "label": "windows",
        "kind": 9
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": []
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
Thing :: plex {
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
        "line": 0,
        "character": 15
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
    "result": []
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
use std.term

main :: fn () {
    term_init()
}

simulate :: fn (s: TermSimulate) {
    s.
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
        "line": 7,
        "character": 6
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
              "line": 8,
              "character": 0
            },
            "end": {
              "line": 8,
              "character": 1
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 8,
                    "character": 0
                  },
                  "end": {
                    "line": 8,
                    "character": 1
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
        "label": "dt",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "width",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "height",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "keyboard",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "events",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "mouse",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "user_data",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "key_down",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "key_pressed",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "key_released",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "key_name",
        "kind": 2,
        "detail": "method"
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
use std.term

simulate :: fn (s: TermSimulate) {
    _ := term_key_down(s.keyboard, Q)
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
        "line": 3,
        "character": 36
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
        "line": 3,
        "character": 35
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
        "label": "Quote",
        "kind": 20
      },
      {
        "label": "Q",
        "kind": 20
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "uri": "file:///home/matt/nerd/mods/std/term/term.n",
      "range": {
        "start": {
          "line": 82,
          "character": 4
        },
        "end": {
          "line": 82,
          "character": 5
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
use test.public_impl

main :: fn () {
    box: Box
    box.
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
        "line": 4,
        "character": 8
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
              "character": 0
            },
            "end": {
              "line": 5,
              "character": 1
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 5,
                    "character": 0
                  },
                  "end": {
                    "line": 5,
                    "character": 1
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
        "label": "value",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "bump",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "get",
        "kind": 2,
        "detail": "method"
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
use test.associated_parts

main :: fn () {
    PartFa
    _factory := PartFactory.
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
        "line": 3,
        "character": 10
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
        "line": 4,
        "character": 28
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
              "character": 0
            },
            "end": {
              "line": 5,
              "character": 1
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 5,
                    "character": 0
                  },
                  "end": {
                    "line": 5,
                    "character": 1
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
        "label": "PartFactory",
        "kind": 22
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": [
      {
        "label": "init",
        "kind": 2,
        "detail": "method"
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
use test.associated_parts

main :: fn () {
    factory := PartFactory.init()
    for factory.
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
        "line": 4,
        "character": 16
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
              "character": 0
            },
            "end": {
              "line": 5,
              "character": 1
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 5,
                    "character": 0
                  },
                  "end": {
                    "line": 5,
                    "character": 1
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
        "label": "init",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "apply",
        "kind": 2,
        "detail": "method"
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
use std.frame

main :: fn () {
    event: FrameEvent = None
    on event {
        None => {
        }
        Closed => {
        }
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
        "line": 7,
        "character": 10
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
        "label": "Closed",
        "kind": 20
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
vertices :: [
    -0.5,
    0.5,
]

main :: fn () {
    vertices.
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
        "line": 6,
        "character": 13
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
              "line": 7,
              "character": 0
            },
            "end": {
              "line": 7,
              "character": 1
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 7,
                    "character": 0
                  },
                  "end": {
                    "line": 7,
                    "character": 1
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

¬
main :: fn () {
    vertex_
    broken(^shader_source)
}

vertex_shader_program ::
    """
    #version 330 core
    """
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
        "line": 1,
        "character": 11
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
              "line": 1,
              "character": 4
            },
            "end": {
              "line": 1,
              "character": 11
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown symbol `vertex_`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 1,
                    "character": 4
                  },
                  "end": {
                    "line": 1,
                    "character": 11
                  }
                }
              },
              "message": "help: Add a binding for `vertex_` or fix the spelling."
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
        "label": "vertex_shader_program",
        "kind": 21
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
main :: fn () {
    fragment_shader_program.
    broken()
}

fragment_shader_program ::
    """
    #version 330 core
    """
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
        "line": 1,
        "character": 28
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
              "line": 2,
              "character": 4
            },
            "end": {
              "line": 2,
              "character": 10
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Type mismatch: expected `string field .data, .count, .bytes, or defined method`, found `broken`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 2,
                    "character": 4
                  },
                  "end": {
                    "line": 2,
                    "character": 10
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

¬
-- Core bindings are implicit.

VAO :: plex {
    id i32
}

impl VAO {
    new :: fn () -> ?Self {
        return Self { id: 1 }
    }

    bind :: fn (self: ^Self) {
    }
}

main :: fn () {
    VAO.new().
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
        "character": 14
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
              "line": 17,
              "character": 0
            },
            "end": {
              "line": 17,
              "character": 1
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 17,
                    "character": 0
                  },
                  "end": {
                    "line": 17,
                    "character": 1
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
    "result": []
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
-- Core bindings are implicit.

VAO :: plex {
    id i32
}

impl VAO {
    new :: fn () -> ?Self {
        return Self { id: 1 }
    }

    bind :: fn (self: ^Self) {
    }
}

main :: fn () {
    on VAO.new() => [vao] {
        vao.
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
        "line": 17,
        "character": 12
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
              "line": 18,
              "character": 4
            },
            "end": {
              "line": 18,
              "character": 5
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 18,
                    "character": 4
                  },
                  "end": {
                    "line": 18,
                    "character": 5
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
        "label": "id",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "bind",
        "kind": 2,
        "detail": "method"
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
-- Core bindings are implicit.

VAO :: plex {
    id i32
}

impl VAO {
    pub new :: fn () -> ?Self {
        return Some(Self { id: 1 })
    }

    bind :: fn (self: ^Self) {
    }
}

main :: fn () {
    VAO.
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
              "line": 17,
              "character": 0
            },
            "end": {
              "line": 17,
              "character": 1
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 17,
                    "character": 0
                  },
                  "end": {
                    "line": 17,
                    "character": 1
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
        "label": "new",
        "kind": 2,
        "detail": "method"
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
-- lsp-uri: __REPO_URI__/tests/lsp/170-source-root-associated-option-self-completion/src/hello.n
-- lsp-root-uri: __REPO_URI__/tests/lsp/170-source-root-associated-option-self-completion
use vao

main :: fn () {
    VAO.
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/completion",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/170-source-root-associated-option-self-completion/src/hello.n"
      },
      "position": {
        "line": 3,
        "character": 8
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
      "uri": "__REPO_URI__/tests/lsp/170-source-root-associated-option-self-completion/src/hello.n",
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 4,
              "character": 0
            },
            "end": {
              "line": 4,
              "character": 1
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Symbol but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/tests/lsp/170-source-root-associated-option-self-completion/src/hello.n",
                "range": {
                  "start": {
                    "line": 4,
                    "character": 0
                  },
                  "end": {
                    "line": 4,
                    "character": 1
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
        "label": "new",
        "kind": 2,
        "detail": "method"
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
use std.frame

main :: fn () {
    event: FrameEvent = None
    on event {
        KeyPress { scan_code: _ } => {
        }
    }
}
¬
[
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
          "range": {
            "start": {
              "line": 5,
              "character": 30
            },
            "end": {
              "line": 5,
              "character": 31
            }
          },
          "text": "E"
        }
      ]
    }
  },
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
        "character": 31
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
        "label": "Enter",
        "kind": 20
      },
      {
        "label": "Escape",
        "kind": 20
      },
      {
        "label": "E",
        "kind": 20
      },
      {
        "label": "End",
        "kind": 20
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
Value :: plex {
}

Choice :: enum {
    First
    Second
}

Thing :: plex {
    value Value
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
        "line": 9,
        "character": 4
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
        "line": 9,
        "character": 7
      }
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
        "line": 9,
        "character": 13
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
    "result": []
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": []
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": [
      {
        "label": "Value",
        "kind": 22
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
-- Core bindings are implicit.

VAO :: plex {
    id i32
}

impl VAO {
    new :: fn () -> ?Self {
        return Self { id: 1 }
    }

    bind :: fn (self: ^Self) {
    }
}

main :: fn () {
    on VAO.new() => [vao] {
        vao.bind()
        vao.missing()
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
        "line": 17,
        "character": 12
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
              "line": 18,
              "character": 12
            },
            "end": {
              "line": 18,
              "character": 19
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown member `missing` for `VAO`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 2,
                    "character": 0
                  },
                  "end": {
                    "line": 2,
                    "character": 3
                  }
                }
              },
              "message": "Type `VAO` is defined here"
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 18,
                    "character": 12
                  },
                  "end": {
                    "line": 18,
                    "character": 19
                  }
                }
              },
              "message": "help: Use a field or method that exists on `VAO`."
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
        "label": "id",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "bind",
        "kind": 2,
        "detail": "method"
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
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

¬
TokenType :: enum { Eof Request Response }

Header :: plex {
    u8 {
        version        : 4 -- Protocol version.
        _              : 2
        kind TokenType : 2
    }
}

main :: fn () {
    header := Header { version: 1 kind: TokenType.Response }
    _ := header.kind
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
        "line": 12,
        "character": 16
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 12,
        "character": 18
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 12,
        "character": 18
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
        "label": "version",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "kind",
        "kind": 5,
        "detail": "field"
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nkind\n```\n\n- Kind: plex field\n- Type: `TokenType`\n- Owner: `Header`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "uri": "file:///test.n",
      "range": {
        "start": {
          "line": 6,
          "character": 8
        },
        "end": {
          "line": 6,
          "character": 12
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
sum :: fn (count: i32, args: ...) -> i32 {
    return count + args.next[i32]()
}
main :: fn () { _value := sum(1, 41) }
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
        "line": 0,
        "character": 1
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 1,
        "character": 20
      }
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
        "line": 1,
        "character": 24
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 3,
        "character": 32
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
        "value": "```nerd\nsum :: fn (count: i32, args: ...) -> i32\n```\n\n- Kind: function"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nargs\n```\n\n- Kind: local variable\n- Type: `VaList`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": [
      {
        "label": "next",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "copy",
        "kind": 2,
        "detail": "method"
      },
      {
        "label": "format",
        "kind": 2,
        "detail": "method"
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "result": {
      "signatures": [
        {
          "label": "sum(count: i32, args: ...) -> i32",
          "documentation": "Named arguments use `name = value`; omitted parameters use declared defaults when available.",
          "parameters": [
            {
              "label": [
                4,
                14
              ]
            },
            {
              "label": [
                16,
                25
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 1
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
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
                    "line": 218,
                    "character": 4
                  },
                  "end": {
                    "line": 218,
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
                    "line": 198,
                    "character": 4
                  },
                  "end": {
                    "line": 198,
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

¬
main :: fn () {
    count := 3
    values: [count]i32
    values[0] = 4
    prn($"{values.count} {values.size}")
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
        "line": 3,
        "character": 6
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
        "line": 4,
        "character": 18
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
        "value": "```nerd\nvalues\n```\n\n- Kind: local variable\n- Type: `[runtime]i32`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
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

¬
Failure :: enum { Bad }
work :: fn () -> i32\Failure { return Failure.Bad! }
check_value :: fn () -> i32\Failure {
    value := work() else [problem] {
        assert problem == Failure.Bad
    }
    return value
}
main :: fn () {}
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
        "line": 4,
        "character": 16
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
        "line": 4,
        "character": 16
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 6,
        "character": 12
      }
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
        "line": 4,
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
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nproblem\n```\n\n- Kind: pattern binder\n- Type: `Failure`"
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
          "character": 26
        },
        "end": {
          "line": 3,
          "character": 33
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nvalue\n```\n\n- Kind: local variable\n- Type: `i32`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "result": [
      {
        "label": "Failure",
        "kind": 22
      },
      {
        "label": "work",
        "kind": 3
      },
      {
        "label": "check_value",
        "kind": 3
      },
      {
        "label": "main",
        "kind": 3
      },
      {
        "label": "problem",
        "kind": 6
      },
      {
        "label": "value",
        "kind": 6
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]
