use std.io

main :: fn () {
    prn("Hello, world!")
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
        "line": 2,
        "character": 0
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
        "character": 4
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
        "line": 3,
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
    "id": 2,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nmain :: fn () -> void\n```\n\n- Kind: function"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nprn_text :: fn (text: string) -> void\n```\n\n- Kind: function\n\nPrints text to standard output and appends a newline."
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\n\"Hello, world!\"\n```\n\n- Type: `string`"
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
use std.io

main :: fn () {
    prn("Hello, World!")
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
        "line": 2,
        "character": 0
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
        "value": "```nerd\nmain :: fn () -> void\n```\n\n- Kind: function"
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
main :: fn () {
    return 42
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
        "line": 0,
        "character": 0
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
        "value": "```nerd\nmain :: fn () -> i32\n```\n\n- Kind: function"
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
value: i32: 120
main: fn () -> i32: fn () => value
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
        "character": 0
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
        "character": 0
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
        "value": "```nerd\nvalue :: 120\n```\n\n- Kind: constant\n- Type: `i32`\n- Value: `120`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nmain :: fn () -> i32\n```\n\n- Kind: function"
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
-- Price stored in minor currency units.
Price :: u16
main :: fn () => 1
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
        "character": 0
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
        "value": "```nerd\nPrice :: u16\n```\n\n- Kind: type alias\n- Type: `u16`\n\nPrice stored in minor currency units."
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
enabled: bool = yes
greeting: string = "Hello"
ratio: f32 = 42.as(f32)
main :: fn () => enabled.as(i32)
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
        "character": 0
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
        "character": 0
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
        "line": 2,
        "character": 0
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
        "value": "```nerd\nenabled\n```\n\n- Kind: variable\n- Type: `bool`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\ngreeting\n```\n\n- Kind: variable\n- Type: `string`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nratio\n```\n\n- Kind: variable\n- Type: `f32`"
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
add :: fn (a: i32, b: i32) => a + b
main :: fn () => add(20, 22)
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
        "character": 0
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
        "value": "```nerd\nadd :: fn (a: i32, b: i32) -> i32\n```\n\n- Kind: function"
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
add :: fn (a: i32, b: i32) => a + b
adder: fn (arg1: i32, arg2: i32) -> i32 = add
main :: fn () => adder(20, 22)
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
        "character": 0
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
        "value": "```nerd\nadder\n```\n\n- Kind: variable\n- Type: `fn (i32, i32) -> i32`"
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
count: i32 = 2
main :: fn () => on count {
    0 => 10
    else => 20
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
        "line": 0,
        "character": 0
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
        "value": "```nerd\ncount\n```\n\n- Kind: variable\n- Type: `i32`"
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
use std.io

make_pair :: fn(a: i32, b: string) -> (i32, string) {
    return (a, b)
}

main :: fn () {
    pair :: make_pair(7, "seven")
    size: (i32, i32) = (80, 25)
    _width := size.0
    prn($"{pair.0} {pair.1}")
    prn($"{size.0}")
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
        "line": 7,
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
        "line": 9,
        "character": 14
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
        "line": 9,
        "character": 19
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
        "value": "```nerd\npair\n```\n\n- Kind: local variable\n- Type: `(i32, string)`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nsize\n```\n\n- Kind: local variable\n- Type: `(i32, i32)`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\n0\n```\n\n- Kind: tuple field\n- Type: `i32`\n- Owner: `(i32, i32)`"
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
use std.io

main :: fn () {
    values: [3]i32 = [1, 2, 3]
    prn($"{values[1]}")
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
        "character": 4
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
        "value": "```nerd\nvalues\n```\n\n- Kind: local variable\n- Type: `[3]i32`"
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
use std.io

main :: fn () {
    values: [3]i32 = [10, 20, 30]
    pointer: ^[3]i32 = ^values
    prn($"{pointer[0]}")
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
        "line": 4,
        "character": 4
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
        "value": "```nerd\npointer\n```\n\n- Kind: local variable\n- Type: `^[3]i32`"
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
use std.io

main :: fn () {
    values: [3]i32 = [1, 2, 3]
    slice: []i32 = values[..]
    prn($"{slice.count}")
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
        "line": 4,
        "character": 4
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
        "value": "```nerd\nslice\n```\n\n- Kind: local variable\n- Type: `[]i32`"
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
use std.io

main :: fn () {
    text: string = "hello"
    ell: string = text[1..4]
    prn($"{ell.count}")
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
        "line": 4,
        "character": 4
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
        "value": "```nerd\nell\n```\n\n- Kind: local variable\n- Type: `string`"
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
use std.io

Value :: union { i i32 f f32 }
main :: fn () {
    v: Value = Value { i: 1 }
    prn($"{v.i}")
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
        "line": 4,
        "character": 4
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
        "value": "```nerd\nv\n```\n\n- Kind: local variable\n- Type: `Value`"
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
use std.io

Colour :: enum { Red Green Blue }
main :: fn () {
    _colour: Colour = Green
    prn("enum")
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
        "line": 4,
        "character": 4
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
        "value": "```nerd\n_colour\n```\n\n- Kind: local variable\n- Type: `Colour`"
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
use std.io

Maybe :: enum { None Some(i32) }
main :: fn () {
    _value: Maybe = Some(1)
    prn("enum payload")
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
        "line": 4,
        "character": 4
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
        "value": "```nerd\n_value\n```\n\n- Kind: local variable\n- Type: `Maybe`"
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
main :: fn () -> i32 {
    value := 7
    return on {
        value < 10 => value
        else => 0
    }
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
        "character": 22
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
        "value": "```nerd\nvalue\n```\n\n- Kind: local variable\n- Type: `i32`"
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
Point :: plex { x i32 y i32 }

main :: fn () {
    p : Point = { x: 1, y: 2 }
    show([p, { x: 3, y: 4 }])
}

show :: fn (_items: []Point) {
    return
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
        "character": 4
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
        "value": "```nerd\np\n```\n\n- Kind: local variable\n- Type: `Point`"
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
add :: fn (a: i32, b: i32 = 1, c: i32 = a + b) => a + b + c
main :: fn () => add(20)
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
        "character": 0
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
        "value": "```nerd\nadd :: fn (a: i32, b: i32 = 1, c: i32 = a + b) -> i32\n```\n\n- Kind: function"
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
id :: fn [T] (value: T) -> T {
    return value
}

main :: fn () => id(1)
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
        "character": 0
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/documentSymbol",
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
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nid :: fn [T] (value: T) -> T\n```\n\n- Kind: function"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": [
      {
        "name": "id",
        "kind": 12,
        "range": {
          "start": {
            "line": 0,
            "character": 0
          },
          "end": {
            "line": 0,
            "character": 2
          }
        },
        "selectionRange": {
          "start": {
            "line": 0,
            "character": 0
          },
          "end": {
            "line": 0,
            "character": 2
          }
        },
        "detail": "function"
      },
      {
        "name": "main",
        "kind": 12,
        "range": {
          "start": {
            "line": 4,
            "character": 0
          },
          "end": {
            "line": 4,
            "character": 4
          }
        },
        "selectionRange": {
          "start": {
            "line": 4,
            "character": 0
          },
          "end": {
            "line": 4,
            "character": 4
          }
        },
        "detail": "function"
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
answer :: 1

main :: fn () => answer
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
              "line": 0,
              "character": 10
            },
            "end": {
              "line": 0,
              "character": 11
            }
          },
          "text": "2"
        }
      ]
    }
  },
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
        "character": 0
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
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nanswer :: 2\n```\n\n- Kind: constant\n- Type: `untyped integer`\n- Value: `2`"
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
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 10,
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
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nframe\n```\n\n- Kind: local variable\n- Type: `^Frame`"
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
-- Core bindings are implicit.

main :: fn () {
    scratch: arena
    scratch = arena(16)
    _ := scratch.mark()
    temp_arena.reset()
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
        "character": 13
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
        "line": 5,
        "character": 17
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
        "character": 4
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 6,
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
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\narena\n```\n\n```nerd\narena :: fn (num_bytes: usize, increment: usize = num_bytes) -> arena\n```\n\n- Kind: built-in type\n- Type: `arena`\n- Notes: opaque, pointer-stable allocation arena\n\nCreates a pointer-stable arena with the requested initial capacity. The optional increment controls how much storage is committed when more space is needed."
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nmark :: fn () -> u32\n```\n\n- Kind: arena method\n- Owner: `arena`\n\nReturns the current arena cursor for a later restore operation."
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\ntemp_arena\n```\n\n- Kind: constant\n- Type: `^arena`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nreset :: fn () -> void\n```\n\n- Kind: arena method\n- Owner: `arena`\n\nInvalidates all allocations from the arena while retaining its storage for reuse."
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
-- Adds two values with an optional adjustment.
-- Returns the adjusted total.
add :: fn (left: i32, right: i32, adjustment: i32 = 0) => left + right + adjustment

main :: fn () => add(1, 2)
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
        "line": 2,
        "character": 0
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
        "value": "```nerd\nadd :: fn (left: i32, right: i32, adjustment: i32 = 0) -> i32\n```\n\n- Kind: function\n\nAdds two values with an optional adjustment.\nReturns the adjusted total."
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
-- lsp-uri: __REPO_URI__/tests/lsp/135-imported-hover-doc-comment.input.n
use imported_doc_comment

main :: fn () => documented(1)
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/135-imported-hover-doc-comment.input.n"
      },
      "position": {
        "line": 2,
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
      "uri": "__REPO_URI__/tests/lsp/135-imported-hover-doc-comment.input.n",
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\ndocumented :: fn (value: i32, extra: i32 = 1) -> i32\n```\n\n- Kind: function\n\nProvides imported hover text."
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
use std.term

main :: fn () {
    term_init()
    missing()
}

simulate :: fn (s: TermSimulate) {
    s.input
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
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 7,
        "character": 20
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
              "character": 4
            },
            "end": {
              "line": 4,
              "character": 11
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown symbol `missing`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 4,
                    "character": 4
                  },
                  "end": {
                    "line": 4,
                    "character": 11
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
    "id": 2,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nterm_init :: fn (min_width: u32 = TERM_DEFAULT_MIN_WIDTH, min_height: u32 = TERM_DEFAULT_MIN_HEIGHT, too_small_policy: TermTooSmallPolicy = TermTooSmallPolicy.PauseSimulation) -> void\n```\n\n- Kind: function\n\nEnters terminal application mode."
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nTermSimulate :: TermSimulate\n```\n\n- Kind: type alias\n- Type: `TermSimulate`\n\nState supplied to a simulation callback for one timed update."
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
FrameSystem :: plex {
    id i32
}

Frame :: plex {
    system ^FrameSystem
    id i32
}

impl FrameSystem {
    apply :: fn (self: ^Self, frame: ^Frame) {
        _ := self
        _ := frame
    }
}

Event :: enum {
    Closed
    KeyPress { scan_code i32 }
}

main :: fn () {
    frame_system := FrameSystem { id: 1 }
    frame := {
        system : ^frame_system
        id     : 2
    }

    frame_system.apply(^frame)

    event := Event.KeyPress { scan_code: 27 }
    on event {
        Closed => {
        }
        KeyPress { scan_code: code } => {
            _ := code
        }
    }
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
        "line": 22,
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
        "line": 23,
        "character": 4
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
        "line": 24,
        "character": 8
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 25,
        "character": 8
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 28,
        "character": 4
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 7,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 28,
        "character": 17
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 8,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 28,
        "character": 24
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 9,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 32,
        "character": 8
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 10,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 34,
        "character": 8
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 11,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 34,
        "character": 19
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 12,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 34,
        "character": 30
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 13,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 35,
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
        "value": "```nerd\nframe_system\n```\n\n- Kind: local variable\n- Type: `FrameSystem`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nframe\n```\n\n- Kind: local variable\n- Type: `Frame`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nsystem\n```\n\n- Kind: plex field\n- Type: `^FrameSystem`\n- Owner: `Frame`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nid\n```\n\n- Kind: plex field\n- Type: `i32`\n- Owner: `Frame`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nframe_system\n```\n\n- Kind: local variable\n- Type: `FrameSystem`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 7,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\napply :: fn (self: ^Self, frame: ^Frame) -> void\n```\n\n- Kind: method"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 8,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nframe\n```\n\n- Kind: local variable\n- Type: `Frame`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 9,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nClosed\n```\n\n- Kind: enum variant\n- Owner: `Event`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 10,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nKeyPress\n```\n\n- Kind: enum variant\n- Owner: `Event`\n- Payload: `plex { i32 scan_code }`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 11,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nscan_code\n```\n\n- Kind: plex field\n- Type: `i32`\n- Owner: `plex { i32 scan_code }`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 12,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\ncode\n```\n\n- Kind: pattern binder\n- Type: `i32`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 13,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\ncode\n```\n\n- Kind: pattern binder\n- Type: `i32`"
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
use test.associated_parts

main :: fn () {
    factory := PartFactory.init()
    factory.apply()
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
        "line": 4,
        "character": 12
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
        "value": "```nerd\napply :: fn (factory: ^Self) -> void\n```\n\n- Kind: method"
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
use std.image

main :: fn () {
    bytes: []u8 = nil
    _result := Image.load_bytes(bytes, desired_channels = 4)
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
        "line": 4,
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
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nload_bytes :: fn (bytes: []u8, desired_channels: i32 = 0) -> Self\\ImageError\n```\n\n- Kind: method\n\nLoads an image from bytes already available in memory."
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
use std.term

input_hook :: fn (input: TermInput) {
    _pressed := input.key_pressed(Escape)
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
        "character": 25
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
        "value": "```nerd\nkey_pressed :: fn (input: Self, key: TermScanCode) -> bool\n```\n\n- Kind: method\n\nReturns whether a physical key was newly pressed in this input snapshot."
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
-- Core bindings are implicit.

main :: fn () {
    for x in [0 .. 4.as(u32)] {
        _ := x
    }
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
        "character": 8
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
        "line": 4,
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
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nx\n```\n\n- Kind: local variable\n- Type: `u32`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nx\n```\n\n- Kind: local variable\n- Type: `u32`"
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
use std.frame
use std.gfx

consume :: fn (_value: u16) {
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
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 11,
        "character": 39
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
        "character": 8
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
        "line": 12,
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
          "message": "Type mismatch: expected `u16`, found `u32`",
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

¬
Event :: enum {
    -- The frame was closed by the user.
    Closed

    -- A physical key was pressed.
    KeyPress { scan_code i32 }
}

main :: fn () {
    event: Event = Closed
    on event {
        Closed => {}
        KeyPress { scan_code: code } => {
            _ := code
        }
    }
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 1,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 11,
        "character": 10
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 12,
        "character": 12
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
    "id": 1,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nClosed\n```\n\n- Kind: enum variant\n- Owner: `Event`\n\nThe frame was closed by the user."
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nKeyPress\n```\n\n- Kind: enum variant\n- Owner: `Event`\n- Payload: `plex { i32 scan_code }`\n\nA physical key was pressed."
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
use std.frame

main :: fn () {
    event: FrameEvent = None
    on event {
        Closed => {}
    }
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
        "line": 5,
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
        "value": "```nerd\nClosed\n```\n\n- Kind: enum variant\n- Owner: `FrameEvent`\n\nThe frame was closed by the user"
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
main :: fn (value: i32\string) {
    on value => {
        _ := value
    } else {
        _ := value
    }
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
        "line": 2,
        "character": 13
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
        "line": 4,
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
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nvalue\n```\n\n- Kind: pattern binder\n- Type: `i32`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nvalue\n```\n\n- Kind: pattern binder\n- Type: `string`"
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
use std.files

inspect :: fn (info: ^FileInfo) {
    _ := info.size
}

main :: fn () {
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
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nsize\n```\n\n- Kind: plex field\n- Type: `u64`\n- Owner: `FileInfo`\n\nObject size in bytes, or zero when unavailable."
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
single :: fn (_value: f32) {}
main :: fn () {
    colour := (0.25, 0.5)
    alias := colour.0
    single(alias)
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
        "line": 2,
        "character": 5
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
        "character": 5
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
        "value": "```nerd\ncolour\n```\n\n- Kind: local variable\n- Type: `(f32, f64)`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nalias\n```\n\n- Kind: local variable\n- Type: `f32`"
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
State :: plex { background_colour (f32, f32, f32) }
draw :: fn (_state: ^State) {}
main :: fn () {
    draw_state := { background_colour: (0.1, 0.2, 0.3) }
    draw(^draw_state)
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
        "value": "```nerd\ndraw_state\n```\n\n- Kind: local variable\n- Type: `State`"
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
use std.frame

main :: fn () {
    frame_system := FrameSystem.init()
    defer frame_system.done()

    main_window := {
        system      : frame_system
        id          : NEW_FRAME
        width       : 800
        height      : 600
        title       : "Vulkan Triangle"
        full_screen : no
        resizable   : yes
    }

    frame_system.apply(^main_window)

    -- Main loop
    for frame_system.loop() {
        frame_system.update(^main_window)
        on frame_system.poll(^main_window) {
            None => {
            }

            Resized { width: _, height: _ } => {
            }

            KeyPress { scan_code: _ } => {
            }

            KeyRelease { scan_code: _ } => {}
            Character { codepoint: _ } => {}
        }
    }
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
        "line": 25,
        "character": 22
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
        "line": 25,
        "character": 32
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
        "line": 28,
        "character": 23
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 31,
        "character": 25
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 32,
        "character": 24
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
          "range": {
            "start": {
              "line": 7,
              "character": 22
            },
            "end": {
              "line": 7,
              "character": 22
            }
          },
          "text": "^"
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 12,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 25,
        "character": 22
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 13,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 25,
        "character": 32
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 14,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 28,
        "character": 23
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 15,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 31,
        "character": 25
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 16,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 32,
        "character": 24
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
              "character": 22
            },
            "end": {
              "line": 7,
              "character": 34
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Type mismatch: expected `^FrameSystem`, found `FrameSystem`",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/mods/std/frame/frame.n",
                "range": {
                  "start": {
                    "line": 149,
                    "character": 4
                  },
                  "end": {
                    "line": 149,
                    "character": 15
                  }
                }
              },
              "message": "Type `FrameSystem` is defined here"
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 7,
                    "character": 22
                  },
                  "end": {
                    "line": 7,
                    "character": 34
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
        "value": "```nerd\nwidth\n```\n\n- Kind: plex field\n- Type: `u16`\n- Owner: `plex { u16 width, u16 height }`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nheight\n```\n\n- Kind: plex field\n- Type: `u16`\n- Owner: `plex { u16 width, u16 height }`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nscan_code\n```\n\n- Kind: plex field\n- Type: `FrameScanCode`\n- Owner: `plex { FrameScanCode scan_code }`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nscan_code\n```\n\n- Kind: plex field\n- Type: `FrameScanCode`\n- Owner: `plex { FrameScanCode scan_code }`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\ncodepoint\n```\n\n- Kind: plex field\n- Type: `u32`\n- Owner: `plex { u32 codepoint }`"
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
    "id": 12,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nwidth\n```\n\n- Kind: plex field\n- Type: `u16`\n- Owner: `plex { u16 width, u16 height }`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 13,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nheight\n```\n\n- Kind: plex field\n- Type: `u16`\n- Owner: `plex { u16 width, u16 height }`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 14,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nscan_code\n```\n\n- Kind: plex field\n- Type: `FrameScanCode`\n- Owner: `plex { FrameScanCode scan_code }`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 15,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nscan_code\n```\n\n- Kind: plex field\n- Type: `FrameScanCode`\n- Owner: `plex { FrameScanCode scan_code }`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 16,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\ncodepoint\n```\n\n- Kind: plex field\n- Type: `u32`\n- Owner: `plex { u32 codepoint }`"
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
Info :: plex { value i32 }
CreateInfo :: plex { info ^Info }
main :: fn () {
    pointer := ^Info { value: 7 }
    create := CreateInfo { info: ^{ value: 11 } }
    assert pointer.value == 7
    assert create.info.value == 11
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
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 3,
        "character": 23
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
        "line": 4,
        "character": 36
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 5,
        "character": 19
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
        "value": "```nerd\npointer\n```\n\n- Kind: local variable\n- Type: `^Info`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nvalue\n```\n\n- Kind: plex field\n- Type: `i32`\n- Owner: `Info`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nvalue\n```\n\n- Kind: plex field\n- Type: `i32`\n- Owner: `Info`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nvalue\n```\n\n- Kind: plex field\n- Type: `i32`\n- Owner: `Info`"
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
step :: fn (ok: bool) -> ?void {
    on !ok => return nil
}
run :: fn () -> ?void {
    step(yes)?
    return
}
main :: fn () {
    on run() => { prn("success") }
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
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nstep :: fn (ok: bool) -> ?void\n```\n\n- Kind: function"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]
