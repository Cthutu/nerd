answer :: 42

main :: fn () => answer
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
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 2,
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
        "value": "```nerd\nmain :: fn () -> i32\n```\n\n- Kind: function"
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
          "line": 0,
          "character": 0
        },
        "end": {
          "line": 0,
          "character": 6
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
main :: fn () {
    _value := 10
    {
        value := 32
        return value
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
        "line": 4,
        "character": 15
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
        "value": "```nerd\nvalue\n```\n\n- Kind: local variable\n- Type: `i32`"
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
          "character": 13
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
main :: fn () {
    result := add(20, 22)
    add :: fn (a: i32, b: i32) => a + b
    return result
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 1,
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
      "uri": "file:///test.n",
      "range": {
        "start": {
          "line": 2,
          "character": 4
        },
        "end": {
          "line": 2,
          "character": 7
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
value: u32 = 2
main :: fn () => on value {
    2 as matched => matched
    else as other => other
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
        "character": 10
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
        "line": 2,
        "character": 21
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
        "line": 2,
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
        "value": "```nerd\nmatched\n```\n\n- Kind: pattern binder\n- Type: `u32`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nmatched\n```\n\n- Kind: pattern binder\n- Type: `u32`"
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
          "line": 2,
          "character": 9
        },
        "end": {
          "line": 2,
          "character": 16
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
use std.io

Point :: plex { x i32 y i32 }

main :: fn () {
    p :: Point { x: 1, y: 2 }
    prn($"{p.x} {p.y}")
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
        "line": 6,
        "character": 13
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
        "line": 6,
        "character": 13
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "method": "textDocument/semanticTokens/full",
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
        "value": "```nerd\np\n```\n\n- Kind: local variable\n- Type: `Point`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nx\n```\n\n- Kind: plex field\n- Type: `i32`\n- Owner: `Point`"
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
          "line": 2,
          "character": 16
        },
        "end": {
          "line": 2,
          "character": 17
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "result": {
      "data": [
        0,
        0,
        3,
        2,
        0,
        0,
        4,
        3,
        0,
        1,
        0,
        3,
        1,
        4,
        0,
        0,
        1,
        2,
        0,
        1,
        2,
        0,
        5,
        0,
        0,
        0,
        6,
        1,
        4,
        0,
        0,
        1,
        1,
        4,
        0,
        0,
        2,
        4,
        2,
        0,
        0,
        7,
        1,
        0,
        0,
        0,
        2,
        3,
        0,
        0,
        0,
        4,
        1,
        0,
        0,
        0,
        2,
        3,
        0,
        0,
        2,
        0,
        4,
        1,
        0,
        0,
        5,
        1,
        4,
        0,
        0,
        1,
        1,
        4,
        0,
        0,
        2,
        2,
        2,
        0,
        1,
        4,
        1,
        0,
        0,
        0,
        2,
        1,
        4,
        0,
        0,
        1,
        1,
        4,
        0,
        0,
        2,
        5,
        0,
        0,
        0,
        8,
        1,
        0,
        0,
        0,
        1,
        1,
        4,
        0,
        0,
        2,
        1,
        3,
        0,
        0,
        3,
        1,
        0,
        0,
        0,
        1,
        1,
        4,
        0,
        0,
        2,
        1,
        3,
        0,
        1,
        4,
        3,
        1,
        0,
        0,
        4,
        2,
        5,
        0,
        0,
        3,
        1,
        0,
        0,
        0,
        1,
        1,
        4,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        2,
        1,
        5,
        0,
        0,
        2,
        1,
        0,
        0,
        0,
        1,
        1,
        4,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        2,
        1,
        5,
        0
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
    (left, right) := (1, "two")
    prn($"{left} {right}")
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
        "character": 19
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
        "value": "```nerd\nleft\n```\n\n- Kind: local variable\n- Type: `i32`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nright\n```\n\n- Kind: local variable\n- Type: `string`"
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
          "line": 3,
          "character": 5
        },
        "end": {
          "line": 3,
          "character": 9
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
value: u32 = 2
main :: fn () => on value {
    2 as matched on matched == 2 => matched
    else as other => other
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
        "character": 20
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
        "line": 2,
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
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nmatched\n```\n\n- Kind: pattern binder\n- Type: `u32`"
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
          "line": 2,
          "character": 9
        },
        "end": {
          "line": 2,
          "character": 16
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
Point :: plex { x i32 y i32 }
main :: fn () => on Point { x: 3, y: 4 } {
    { x: 3, y: as y } => y
    else => 0
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
        "character": 19
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
        "line": 2,
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
    "result": null
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": null
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
use std.io

abs_local :: ffi "c" abs (value: i32) -> i32

main :: fn () {
    value := abs_local(-7)
    prn($"abs = {value}")
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
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 5,
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
        "value": "```nerd\nabs_local :: fn (value: i32) -> i32\n```\n\n- Kind: function"
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
          "line": 2,
          "character": 0
        },
        "end": {
          "line": 2,
          "character": 9
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
memory :: use std.memory

main :: fn () {
    _ := memory.kb(2)
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
        "character": 16
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
        "character": 16
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "method": "textDocument/definition",
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
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nmemory\n```\n\n- Kind: module\n- Type: `module`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nkb :: fn (n: usize) -> usize\n```\n\n- Kind: function\n\nConverts kibibytes to bytes."
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "uri": "file:///home/matt/nerd/mods/std/memory.n",
      "range": {
        "start": {
          "line": 308,
          "character": 4
        },
        "end": {
          "line": 308,
          "character": 6
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "result": {
      "uri": "file:///home/matt/nerd/mods/std/memory.n",
      "range": {
        "start": {
          "line": 0,
          "character": 0
        },
        "end": {
          "line": 0,
          "character": 0
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
use std.io

fcntl_local :: ffi "c" fcntl (fd: i32, command: i32, ...) -> i32

main :: fn () {
    result := fcntl_local(0, 1, 0)
    prn($"fcntl = {result >= 0}")
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
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 5,
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
        "value": "```nerd\nfcntl_local :: fn (fd: i32, command: i32, ...) -> i32\n```\n\n- Kind: function"
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
          "line": 2,
          "character": 0
        },
        "end": {
          "line": 2,
          "character": 11
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
use std.io

main :: fn () {
    prn("Hello")
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
        "character": 4
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
        "line": 0,
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
        "value": "```nerd\nprn_text :: fn (text: string) -> void\n```\n\n- Kind: function\n\nPrints text to standard output and appends a newline."
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "uri": "__REPO_URI__/mods/core.n",
      "range": {
        "start": {
          "line": 89,
          "character": 0
        },
        "end": {
          "line": 89,
          "character": 8
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "uri": "__REPO_URI__/mods/std/io.n",
      "range": {
        "start": {
          "line": 0,
          "character": 0
        },
        "end": {
          "line": 0,
          "character": 0
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
-- lsp-config: tests/lsp/config/nerd.json
on "lsp_config_feature" {
    answer :: 7
}

main :: fn () => answer
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 4,
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
      "uri": "file:///test.n",
      "range": {
        "start": {
          "line": 1,
          "character": 4
        },
        "end": {
          "line": 1,
          "character": 10
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

main :: fn () {
    _p : Point  = Point { x: 1, y: 2 }
    _c : Colour = Colour.RED
    _v : Value  = Value { i: 1 }
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 3,
        "character": 9
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
        "character": 9
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
        "line": 5,
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
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": {
      "uri": "__REPO_URI__/tests/mods/test/lsp_types.n",
      "range": {
        "start": {
          "line": 0,
          "character": 4
        },
        "end": {
          "line": 0,
          "character": 9
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "uri": "__REPO_URI__/tests/mods/test/lsp_types.n",
      "range": {
        "start": {
          "line": 5,
          "character": 4
        },
        "end": {
          "line": 5,
          "character": 10
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "uri": "__REPO_URI__/tests/mods/test/lsp_types.n",
      "range": {
        "start": {
          "line": 10,
          "character": 4
        },
        "end": {
          "line": 10,
          "character": 9
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
State :: plex { inventory i32 }

main :: fn () {
    game :: State { inventory: 1 }
    _ := game.inventory
}
¬
[
  {
    "jsonrpc": "2.0",
    "method": "textDocument/didChange",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "contentChanges": [
        {
          "text": "State :: plex { inventory i32 }\n\nmain :: fn () {\n    game :: State { inventory: 1 }\n    _ := game.inventory\n    broken :\n}\n"
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 4,
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
    "method": "textDocument/publishDiagnostics",
    "params": {
      "uri": "file:///test.n",
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 6,
              "character": 0
            },
            "end": {
              "line": 6,
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
                    "line": 6,
                    "character": 0
                  },
                  "end": {
                    "line": 6,
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
    "result": {
      "uri": "file:///test.n",
      "range": {
        "start": {
          "line": 0,
          "character": 16
        },
        "end": {
          "line": 0,
          "character": 25
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
use test { grouped { alpha } }

main :: fn () {
    _ := alpha()
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
        "character": 9
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
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nalpha :: fn () -> i32\n```\n\n- Kind: function"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "uri": "__REPO_URI__/tests/mods/test/grouped/alpha.n",
      "range": {
        "start": {
          "line": 0,
          "character": 4
        },
        "end": {
          "line": 0,
          "character": 9
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
Thing :: plex {
    size (i32, i32)
}

main :: fn () {
    thing: Thing
    thing.size = (7, 5)
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 6,
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
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": {
      "uri": "file:///test.n",
      "range": {
        "start": {
          "line": 1,
          "character": 4
        },
        "end": {
          "line": 1,
          "character": 8
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
main :: fn () => 0
¬
[
  {
    "jsonrpc": "2.0",
    "method": "textDocument/didOpen",
    "params": {
      "textDocument": {
        "uri": "file:///home/matt/nerd/tests/mods/test/parts/body.n",
        "languageId": "nerd",
        "version": 1,
        "text": "pub make_thing :: fn (value: i32) -> Thing {\n    return Thing { value: value }\n}\n\npub part_answer :: fn () -> i32 {\n    thing := make_thing(42)\n    return thing.value\n}\n"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///home/matt/nerd/tests/mods/test/parts/body.n"
      },
      "position": {
        "line": 6,
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
    "method": "textDocument/publishDiagnostics",
    "params": {
      "uri": "file:///home/matt/nerd/tests/mods/test/parts/body.n",
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": {
      "uri": "file:///home/matt/nerd/tests/mods/test/parts/body.n",
      "range": {
        "start": {
          "line": 5,
          "character": 4
        },
        "end": {
          "line": 5,
          "character": 9
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
Arena :: plex [T] {
    value T
}

SimpleArena :: Arena[u8]

bad := Missing

main :: fn () => 0
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 4,
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
              "line": 6,
              "character": 7
            },
            "end": {
              "line": 6,
              "character": 14
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown symbol `Missing`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 6,
                    "character": 7
                  },
                  "end": {
                    "line": 6,
                    "character": 14
                  }
                }
              },
              "message": "help: Add a binding for `Missing` or fix the spelling."
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
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
boxmod :: use test.imported_plex

Holder :: plex {
    box boxmod.Box
    ptr ^boxmod.Box
}

main :: fn () -> i32 {
    box := boxmod.make_box(7)
    holder := Holder { box, ptr: ^box }
    return (holder.box.value + holder.ptr^.value).as(i32)
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/definition",
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
    "id": 2,
    "result": {
      "uri": "__REPO_URI__/tests/mods/test/imported_plex.n",
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
    frames [..]i32
}

main :: fn () {
}

use_frame :: fn (fs: ^FrameSystem) {
    fs.frames
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
        "line": 8,
        "character": 7
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
        "line": 8,
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
              "line": 8,
              "character": 4
            },
            "end": {
              "line": 8,
              "character": 6
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expression result of type `[..]i32` is not used",
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
                    "character": 6
                  }
                }
              },
              "message": "note: Only `void` expressions can be used as standalone statements."
            },
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
                    "character": 6
                  }
                }
              },
              "message": "help: Bind the result to `_` when the value is intentionally ignored."
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
        "value": "```nerd\nframes\n```\n\n- Kind: plex field\n- Type: `[..]i32`\n- Owner: `FrameSystem`"
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
          "line": 1,
          "character": 4
        },
        "end": {
          "line": 1,
          "character": 10
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
    "method": "textDocument/definition",
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
      "uri": "file:///test.n",
      "range": {
        "start": {
          "line": 9,
          "character": 12
        },
        "end": {
          "line": 9,
          "character": 17
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
Display :: trait {
    show :: fn (self: Self) -> string
}

Point :: plex { x i32 y i32 }

impl Display for Point {
    show :: fn (self: Self) -> string {
        return $"({self.x}, {self.y})"
    }
}

main :: fn () {
    p := Point { x: 1, y: 2 }
    prn(p.show())
    prn(Display.show(p))
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/documentSymbol",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
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
        "line": 0,
        "character": 1
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
        "line": 1,
        "character": 5
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
        "line": 7,
        "character": 5
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 14,
        "character": 11
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 7,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 15,
        "character": 17
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 8,
    "method": "textDocument/semanticTokens/full",
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
    "result": [
      {
        "name": "Display",
        "kind": 11,
        "range": {
          "start": {
            "line": 0,
            "character": 0
          },
          "end": {
            "line": 0,
            "character": 7
          }
        },
        "selectionRange": {
          "start": {
            "line": 0,
            "character": 0
          },
          "end": {
            "line": 0,
            "character": 7
          }
        },
        "detail": "trait"
      },
      {
        "name": "Point",
        "kind": 14,
        "range": {
          "start": {
            "line": 4,
            "character": 0
          },
          "end": {
            "line": 4,
            "character": 5
          }
        },
        "selectionRange": {
          "start": {
            "line": 4,
            "character": 0
          },
          "end": {
            "line": 4,
            "character": 5
          }
        },
        "detail": "type alias"
      },
      {
        "name": "impl Display for Point",
        "kind": 11,
        "range": {
          "start": {
            "line": 6,
            "character": 0
          },
          "end": {
            "line": 6,
            "character": 4
          }
        },
        "selectionRange": {
          "start": {
            "line": 6,
            "character": 0
          },
          "end": {
            "line": 6,
            "character": 4
          }
        },
        "detail": "impl"
      },
      {
        "name": "main",
        "kind": 12,
        "range": {
          "start": {
            "line": 12,
            "character": 0
          },
          "end": {
            "line": 12,
            "character": 4
          }
        },
        "selectionRange": {
          "start": {
            "line": 12,
            "character": 0
          },
          "end": {
            "line": 12,
            "character": 4
          }
        },
        "detail": "function"
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nDisplay :: trait\n```\n\n- Kind: trait"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nshow :: fn (self: Self) -> string\n```\n\n- Kind: trait member"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nshow :: fn (self: Self) -> string\n```\n\n- Kind: impl method"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "result": {
      "uri": "file:///test.n",
      "range": {
        "start": {
          "line": 7,
          "character": 4
        },
        "end": {
          "line": 7,
          "character": 8
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 7,
    "result": {
      "uri": "file:///test.n",
      "range": {
        "start": {
          "line": 7,
          "character": 4
        },
        "end": {
          "line": 7,
          "character": 8
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 8,
    "result": {
      "data": [
        0,
        0,
        7,
        0,
        0,
        0,
        8,
        1,
        4,
        0,
        0,
        1,
        1,
        4,
        0,
        0,
        2,
        5,
        2,
        0,
        1,
        4,
        4,
        1,
        0,
        0,
        5,
        1,
        4,
        0,
        0,
        1,
        1,
        4,
        0,
        0,
        2,
        2,
        2,
        0,
        0,
        4,
        4,
        0,
        0,
        0,
        4,
        1,
        4,
        0,
        0,
        2,
        4,
        2,
        0,
        0,
        6,
        2,
        4,
        0,
        0,
        3,
        6,
        0,
        0,
        3,
        0,
        5,
        0,
        0,
        0,
        6,
        1,
        4,
        0,
        0,
        1,
        1,
        4,
        0,
        0,
        2,
        4,
        2,
        0,
        0,
        7,
        1,
        0,
        0,
        0,
        2,
        3,
        0,
        0,
        0,
        4,
        1,
        0,
        0,
        0,
        2,
        3,
        0,
        0,
        2,
        0,
        4,
        2,
        0,
        0,
        5,
        7,
        0,
        0,
        0,
        8,
        3,
        2,
        0,
        0,
        4,
        5,
        0,
        0,
        1,
        4,
        4,
        1,
        0,
        0,
        5,
        1,
        4,
        0,
        0,
        1,
        1,
        4,
        0,
        0,
        2,
        2,
        2,
        0,
        0,
        4,
        4,
        0,
        0,
        0,
        4,
        1,
        4,
        0,
        0,
        2,
        4,
        2,
        0,
        0,
        6,
        2,
        4,
        0,
        0,
        3,
        6,
        0,
        0,
        1,
        8,
        6,
        2,
        0,
        0,
        7,
        2,
        5,
        0,
        0,
        2,
        1,
        5,
        0,
        0,
        2,
        4,
        0,
        0,
        0,
        4,
        1,
        4,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        2,
        2,
        5,
        0,
        0,
        3,
        4,
        0,
        0,
        0,
        4,
        1,
        4,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        2,
        2,
        5,
        0,
        0,
        1,
        1,
        5,
        0,
        4,
        0,
        4,
        1,
        0,
        0,
        5,
        1,
        4,
        0,
        0,
        1,
        1,
        4,
        0,
        0,
        2,
        2,
        2,
        0,
        1,
        4,
        1,
        0,
        0,
        0,
        2,
        1,
        4,
        0,
        0,
        3,
        5,
        0,
        0,
        0,
        8,
        1,
        0,
        0,
        0,
        1,
        1,
        4,
        0,
        0,
        2,
        1,
        3,
        0,
        0,
        3,
        1,
        0,
        0,
        0,
        1,
        1,
        4,
        0,
        0,
        2,
        1,
        3,
        0,
        1,
        4,
        3,
        1,
        0,
        0,
        4,
        1,
        0,
        0,
        0,
        1,
        1,
        4,
        0,
        0,
        1,
        4,
        1,
        0,
        1,
        4,
        3,
        1,
        0,
        0,
        4,
        7,
        0,
        0,
        0,
        7,
        1,
        4,
        0,
        0,
        1,
        4,
        1,
        0,
        0,
        5,
        1,
        0,
        0
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
    "method": "textDocument/definition",
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
    "method": "textDocument/definition",
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
      "uri": "file:///home/matt/nerd/mods/std/term/term.linux.n",
      "range": {
        "start": {
          "line": 679,
          "character": 4
        },
        "end": {
          "line": 679,
          "character": 13
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "uri": "file:///home/matt/nerd/mods/std/term/term.n",
      "range": {
        "start": {
          "line": 270,
          "character": 4
        },
        "end": {
          "line": 270,
          "character": 16
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
Counter :: plex {
    value i32
}

impl Counter {
    -- Increases the counter by `amount`.
    inc :: fn (self: ^Self, amount: i32) {
        self.value += amount
    }
}

get_size :: fn () -> (i32, i32) {
    return (80, 25)
}

test "tuple hover inside source test" {
    size := get_size()

    assert size.0 >= 0
    counter := Counter { value: 0 }
    counter.inc(1)
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
        "line": 16,
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
        "line": 18,
        "character": 11
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
        "line": 18,
        "character": 16
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
        "line": 20,
        "character": 13
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 16,
        "character": 14
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 7,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 20,
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
        "value": "```nerd\nsize\n```\n\n- Kind: local variable\n- Type: `(i32, i32)`"
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
    "id": 5,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\ninc :: fn (self: ^Self, amount: i32) -> void\n```\n\n- Kind: method\n\nIncreases the counter by `amount`."
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "result": {
      "uri": "file:///test.n",
      "range": {
        "start": {
          "line": 11,
          "character": 0
        },
        "end": {
          "line": 11,
          "character": 8
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 7,
    "result": {
      "uri": "file:///test.n",
      "range": {
        "start": {
          "line": 6,
          "character": 4
        },
        "end": {
          "line": 6,
          "character": 7
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
use std.frame

main :: fn () {
    event := FrameEvent.Character { codepoint: 65 }
    _ := event
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/definition",
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
      "uri": "__REPO_URI__/mods/std/frame/frame.n",
      "range": {
        "start": {
          "line": 212,
          "character": 4
        },
        "end": {
          "line": 212,
          "character": 13
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
use std.frame

main :: fn () {
    event : FrameEvent
    on event {
        KeyPress { scan_code } => {
            _ := scan_code
        }
    }
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 5,
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
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": {
      "uri": "__REPO_URI__/mods/std/frame/frame.n",
      "range": {
        "start": {
          "line": 200,
          "character": 4
        },
        "end": {
          "line": 200,
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
use test.parts

main :: fn () -> i32 {
    return part_answer()
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 3,
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
      "uri": "__REPO_URI__/tests/mods/test/parts/body.n",
      "range": {
        "start": {
          "line": 4,
          "character": 4
        },
        "end": {
          "line": 4,
          "character": 15
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
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 7,
        "character": 22
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
        "line": 7,
        "character": 22
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
        "line": 8,
        "character": 18
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 8,
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
      "uri": "__REPO_URI__/mods/std/gfx/mod.n",
      "range": {
        "start": {
          "line": 648,
          "character": 8
        },
        "end": {
          "line": 648,
          "character": 26
        }
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
      "uri": "__REPO_URI__/mods/std/gfx/mod.n",
      "range": {
        "start": {
          "line": 673,
          "character": 8
        },
        "end": {
          "line": 673,
          "character": 23
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
use test.broken_import_exports
use std.opengl
use private_hint_a
gl :: use std.opengl
use missing_module_for_definition
main :: fn () {}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 0,
        "character": 10
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
        "line": 1,
        "character": 4
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
        "line": 1,
        "character": 8
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 1,
        "character": 13
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "method": "textDocument/definition",
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
    "id": 7,
    "method": "textDocument/definition",
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
    "id": 8,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 3,
        "character": 14
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 9,
    "method": "textDocument/definition",
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
    "result": {
      "uri": "__REPO_URI__/tests/mods/test/broken_import_exports.n",
      "range": {
        "start": {
          "line": 0,
          "character": 0
        },
        "end": {
          "line": 0,
          "character": 0
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "uri": "__REPO_URI__/mods/std/opengl/mod.n",
      "range": {
        "start": {
          "line": 0,
          "character": 0
        },
        "end": {
          "line": 0,
          "character": 0
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "uri": "__REPO_URI__/mods/std/opengl/mod.n",
      "range": {
        "start": {
          "line": 0,
          "character": 0
        },
        "end": {
          "line": 0,
          "character": 0
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "result": {
      "uri": "__REPO_URI__/mods/std/opengl/mod.n",
      "range": {
        "start": {
          "line": 0,
          "character": 0
        },
        "end": {
          "line": 0,
          "character": 0
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "result": {
      "uri": "__REPO_URI__/tests/mods/private_hint_a.n",
      "range": {
        "start": {
          "line": 0,
          "character": 0
        },
        "end": {
          "line": 0,
          "character": 0
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 7,
    "result": {
      "uri": "__REPO_URI__/mods/std/opengl/mod.n",
      "range": {
        "start": {
          "line": 0,
          "character": 0
        },
        "end": {
          "line": 0,
          "character": 0
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 8,
    "result": {
      "uri": "__REPO_URI__/mods/std/opengl/mod.n",
      "range": {
        "start": {
          "line": 0,
          "character": 0
        },
        "end": {
          "line": 0,
          "character": 0
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 9,
    "result": null
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
-- lsp-uri: __REPO_URI__/tests/lsp/215-result-type-part-definition/api.n
pub fetch :: fn () -> Value\Failure { return Value { number: 1 } }
pub other :: fn () -> Value\OtherFailure { return Value { number: 2 } }
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/215-result-type-part-definition/api.n"
      },
      "position": {
        "line": 0,
        "character": 23
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/215-result-type-part-definition/api.n"
      },
      "position": {
        "line": 0,
        "character": 29
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/215-result-type-part-definition/api.n"
      },
      "position": {
        "line": 0,
        "character": 5
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "method": "textDocument/definition",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/215-result-type-part-definition/api.n"
      },
      "position": {
        "line": 1,
        "character": 29
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
      "uri": "__REPO_URI__/tests/lsp/215-result-type-part-definition/api.n",
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": {
      "uri": "__REPO_URI__/tests/lsp/215-result-type-part-definition/mod.n",
      "range": {
        "start": {
          "line": 0,
          "character": 4
        },
        "end": {
          "line": 0,
          "character": 9
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "uri": "__REPO_URI__/tests/lsp/215-result-type-part-definition/mod.n",
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
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "uri": "__REPO_URI__/tests/lsp/215-result-type-part-definition/api.n",
      "range": {
        "start": {
          "line": 0,
          "character": 4
        },
        "end": {
          "line": 0,
          "character": 9
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "result": {
      "uri": "__REPO_URI__/tests/lsp/215-result-type-part-definition/types.n",
      "range": {
        "start": {
          "line": 0,
          "character": 4
        },
        "end": {
          "line": 0,
          "character": 16
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
