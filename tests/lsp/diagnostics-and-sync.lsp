main :: fn () => answer
¬
[]

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
              "character": 17
            },
            "end": {
              "line": 0,
              "character": 23
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown symbol `answer`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 0,
                    "character": 17
                  },
                  "end": {
                    "line": 0,
                    "character": 23
                  }
                }
              },
              "message": "help: Add a binding for `answer` or fix the spelling."
            }
          ]
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
Price :: Cost
Cost :: Price
¬
[]

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
              "character": 4
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Type alias cycle involving `Cost`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 0,
                    "character": 0
                  },
                  "end": {
                    "line": 0,
                    "character": 5
                  }
                }
              },
              "message": "Cycle closes through `Price` here"
            },
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
                    "character": 4
                  }
                }
              },
              "message": "help: Break the cycle by rewriting one of the aliases so it resolves to a concrete type."
            }
          ]
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
count := 3
message := $"count={count}"
¬
[]

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
              "character": 11
            },
            "end": {
              "line": 1,
              "character": 13
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Runtime interpolated strings cannot be top-level values",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 1,
                    "character": 11
                  },
                  "end": {
                    "line": 1,
                    "character": 13
                  }
                }
              },
              "message": "help: Use only compile-time values in top-level interpolated strings, or move the interpolation into a function."
            }
          ]
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
use std.io

main :: fn () {
    i :: 2

    prn(on i {
        0 => "zero"
        else as x => $"non-zero: {x}"
    })
}
¬
[]

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
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
use std.io

main :: fn () {
    value :: 2

    on value {
        1 => prn("one")
    }
}
¬
[]

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
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
value: i32 = 2
main :: fn () => on value {
    1 => "one"
}
¬
[]

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
              "character": 17
            },
            "end": {
              "line": 1,
              "character": 19
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Value-producing block-form `on` expressions must be exhaustive",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 1,
                    "character": 17
                  },
                  "end": {
                    "line": 1,
                    "character": 19
                  }
                }
              },
              "message": "help: Add an `else` branch, or use this `on` as a statement when missing cases should be a no-op."
            }
          ]
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
main :: fn () {
    for {
        return 7
    }
}
¬
[]

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
    "id": 999,
    "result": null
  }
]

¬
main :: fn () {
    i := 0
    for i < 5 {
        i += 1
    }
}
¬
[]

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
    "id": 999,
    "result": null
  }
]

¬
use std.io

main :: fn () {
    _total := 0
    for i := 0, step := 1; i < 5; _total += step, i += 1 {
        prn($"C {i}")
    }
}
¬
[]

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
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
main :: fn () {
    value := 1
}
¬
[]

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
              "character": 9
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unused local variable `value`",
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
                    "character": 9
                  }
                }
              },
              "message": "note: Assigning to a variable does not count as using it."
            },
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
                    "character": 9
                  }
                }
              },
              "message": "help: Remove `value` or prefix the name with `_` if it is deliberately unused."
            }
          ]
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
Location :: plex {
    description string
}

locs: []Location = [
    { description : "field" },
]

main :: fn () {
    player_loc: usize = 1
    for i, loc in locs {
        on {
            i == player_loc => prn("same")
            else => prn($"You go to {loc.description}.")
        }
    }
}
¬
[]

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
    "id": 999,
    "result": null
  }
]

¬
Thing :: plex {
    value i32
}
¬
[]

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
    "id": 999,
    "result": null
  }
]

¬
main :: fn () => 0
¬
[
  {
    "jsonrpc": "2.0",
    "method": "textDocument/didOpen",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/mods/test/parts/body.n",
        "languageId": "nerd",
        "version": 1,
        "text": "pub make_thing :: fn (value: MissingType) -> Thing {\n    return Thing { value: value }\n}\n\npub part_answer :: fn () -> i32 {\n    return 42\n}\n"
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
      "uri": "__REPO_URI__/tests/mods/test/parts/body.n",
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 0,
              "character": 29
            },
            "end": {
              "line": 0,
              "character": 40
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown type `MissingType`",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/tests/mods/test/parts/body.n",
                "range": {
                  "start": {
                    "line": 0,
                    "character": 29
                  },
                  "end": {
                    "line": 0,
                    "character": 40
                  }
                }
              },
              "message": "help: Use a defined type name, or one of the built-in primitive types."
            }
          ]
        }
      ]
    }
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
        "uri": "__REPO_URI__/tests/lsp/079-imported-diagnostic-uri/main.n",
        "languageId": "nerd",
        "version": 1,
        "text": "bad :: use bad\n\nmain :: fn () {}\n"
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
      "uri": "__REPO_URI__/tests/lsp/079-imported-diagnostic-uri/main.n",
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "method": "textDocument/publishDiagnostics",
    "params": {
      "uri": "__REPO_URI__/tests/lsp/079-imported-diagnostic-uri/bad/mod.n",
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 0,
              "character": 13
            },
            "end": {
              "line": 0,
              "character": 20
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown symbol `missing`",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/tests/lsp/079-imported-diagnostic-uri/bad/mod.n",
                "range": {
                  "start": {
                    "line": 0,
                    "character": 13
                  },
                  "end": {
                    "line": 0,
                    "character": 20
                  }
                }
              },
              "message": "help: Add a binding for `missing` or fix the spelling."
            }
          ]
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
pragma unknown(1, 2.5, "text", yes, no)

main :: fn () => 0
¬
[]

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
    "id": 999,
    "result": null
  }
]

¬
-- lsp-uri: __REPO_URI__/tests/mods/test/embed_forms/main.n
main :: fn () {
    bytes := @embed("data.bin")
    _ = bytes.count
}
¬
[]

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
      "uri": "__REPO_URI__/tests/mods/test/embed_forms/main.n",
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
impl [T] []T {
    contains :: fn (values: Self, expected: T) -> bool {
        return values[0] == expected
    }
}

main :: fn () => 0
¬
[]

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
              "character": 25
            },
            "end": {
              "line": 2,
              "character": 27
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Generic parameter `T` requires an `Eq` constraint",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 2,
                    "character": 25
                  },
                  "end": {
                    "line": 2,
                    "character": 27
                  }
                }
              },
              "message": "help: Add `where T: Eq` to the generic function or impl."
            }
          ]
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
impl [T] []T
where T: Eq {
    contains :: fn (self: ^Self, value: T) -> bool {
        for i in [0 .. self.count] {
            on self^[i] == value => return yes
        }
        return no
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
    "id": 999,
    "result": null
  }
]

¬
impl [T] []T {
    contains :: fn (values: ^Self, expected: T) -> bool {
        return values^[0] == expected
    }
}

main :: fn () => 0
¬
[]

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
              "character": 26
            },
            "end": {
              "line": 2,
              "character": 28
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Generic parameter `T` requires an `Eq` constraint",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 2,
                    "character": 26
                  },
                  "end": {
                    "line": 2,
                    "character": 28
                  }
                }
              },
              "message": "help: Add `where T: Eq` to the generic function or impl."
            }
          ]
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
Render :: trait { render :: fn (self: ^Self) }
Thing :: plex { value i32 }
impl Render for Thing { render :: fn (self: ^Self) {} }
main :: fn () {}
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
          "text": "Render :: trait { render :: fn (self: ^Self) }\nThing :: plex { value i32 }\nimpl Render for Thing { render :: fn (_self: ^Self) {} }\nmain :: fn () {}\n"
        }
      ]
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
              "character": 38
            },
            "end": {
              "line": 2,
              "character": 42
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unused parameter `self`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 2,
                    "character": 38
                  },
                  "end": {
                    "line": 2,
                    "character": 42
                  }
                }
              },
              "message": "note: Assigning to a variable does not count as using it."
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 2,
                    "character": 38
                  },
                  "end": {
                    "line": 2,
                    "character": 42
                  }
                }
              },
              "message": "help: Remove `self` or prefix the name with `_` if it is deliberately unused."
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

¬
use private_hint_a
use private_hint_b
main :: fn () { _value: VertexArray }
¬
[]

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
              "character": 24
            },
            "end": {
              "line": 2,
              "character": 35
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown type `VertexArray`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 2,
                    "character": 24
                  },
                  "end": {
                    "line": 2,
                    "character": 35
                  }
                }
              },
              "message": "help: Use a defined type name, or one of the built-in primitive types."
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 2,
                    "character": 24
                  },
                  "end": {
                    "line": 2,
                    "character": 35
                  }
                }
              },
              "message": "help: Used module `private_hint_a` declares `VertexArray` privately. Did you intend to mark that declaration `pub`?"
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 2,
                    "character": 24
                  },
                  "end": {
                    "line": 2,
                    "character": 35
                  }
                }
              },
              "message": "help: Used module `private_hint_b` declares `VertexArray` privately. Did you intend to mark that declaration `pub`?"
            }
          ]
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
build {
    define: editor_feature
    library_path: $NERD_TEST_UNINSTALLED_SDK/Lib
}
on "editor_feature" {
    answer :: 42
}
main :: fn () => answer
¬
[]

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
    "id": 999,
    "result": null
  }
]
