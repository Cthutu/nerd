answer :: 42

main :: fn () => answer
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
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
    "result": {
      "data": [
        0,
        0,
        6,
        0,
        0,
        0,
        7,
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
        3,
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
        0,
        6,
        2,
        4,
        0,
        0,
        3,
        6,
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
enabled: bool = yes
main :: fn () => on enabled => 42 else 7
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
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
    "result": {
      "data": [
        0,
        0,
        7,
        0,
        0,
        0,
        7,
        1,
        4,
        0,
        0,
        2,
        4,
        0,
        0,
        0,
        7,
        3,
        2,
        0,
        1,
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
        0,
        6,
        2,
        4,
        0,
        0,
        3,
        2,
        2,
        0,
        0,
        3,
        7,
        0,
        0,
        0,
        8,
        2,
        4,
        0,
        0,
        3,
        2,
        3,
        0,
        0,
        3,
        4,
        2,
        0,
        0,
        5,
        1,
        3,
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
ratio: f64 = 1.5
main :: fn () => on (ratio >= 1.0 && !no) => 1 else 0
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
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
    "result": {
      "data": [
        0,
        0,
        5,
        0,
        0,
        0,
        5,
        1,
        4,
        0,
        0,
        2,
        3,
        0,
        0,
        0,
        6,
        3,
        3,
        0,
        1,
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
        0,
        6,
        2,
        4,
        0,
        0,
        3,
        2,
        2,
        0,
        0,
        4,
        5,
        0,
        0,
        0,
        9,
        3,
        3,
        0,
        0,
        8,
        2,
        2,
        0,
        0,
        4,
        2,
        4,
        0,
        0,
        3,
        1,
        3,
        0,
        0,
        2,
        4,
        2,
        0,
        0,
        5,
        1,
        3,
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
main :: fn () {
    _total := 0
    for i := 0; i < 6; i += 1 {
        on i == 2 => again
        on i == 5 => break
        _total += i
    }
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
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
    "result": {
      "data": [
        0,
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
        6,
        0,
        0,
        0,
        7,
        1,
        4,
        0,
        0,
        3,
        1,
        3,
        0,
        1,
        4,
        3,
        2,
        0,
        0,
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
        1,
        3,
        0,
        0,
        3,
        1,
        0,
        0,
        0,
        4,
        1,
        3,
        0,
        0,
        3,
        1,
        0,
        0,
        0,
        2,
        2,
        4,
        0,
        0,
        3,
        1,
        3,
        0,
        1,
        8,
        2,
        2,
        0,
        0,
        3,
        1,
        0,
        0,
        0,
        5,
        1,
        3,
        0,
        0,
        2,
        2,
        4,
        0,
        0,
        3,
        5,
        2,
        0,
        1,
        8,
        2,
        2,
        0,
        0,
        3,
        1,
        0,
        0,
        0,
        5,
        1,
        3,
        0,
        0,
        2,
        2,
        4,
        0,
        0,
        3,
        5,
        2,
        0,
        1,
        8,
        6,
        0,
        0,
        0,
        7,
        2,
        4,
        0,
        0,
        3,
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
main :: fn () => ${
    break 7
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
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
    "result": {
      "data": [
        0,
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
        0,
        6,
        2,
        4,
        0,
        0,
        3,
        1,
        4,
        0,
        1,
        4,
        5,
        2,
        0,
        0,
        6,
        1,
        3,
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
main :: fn () {
    for i := 0; i < 3; i += 1 $outer {
        on i == 1 => again $outer
        break $outer
    }
    value :: for $value {
        break $value 7
    }
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
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
    "result": {
      "data": [
        0,
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
        3,
        2,
        0,
        0,
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
        1,
        3,
        0,
        0,
        3,
        1,
        0,
        0,
        0,
        4,
        1,
        3,
        0,
        0,
        3,
        1,
        0,
        0,
        0,
        2,
        2,
        4,
        0,
        0,
        3,
        1,
        3,
        0,
        0,
        2,
        1,
        4,
        0,
        0,
        1,
        5,
        0,
        0,
        1,
        8,
        2,
        2,
        0,
        0,
        3,
        1,
        0,
        0,
        0,
        5,
        1,
        3,
        0,
        0,
        2,
        2,
        4,
        0,
        0,
        3,
        5,
        2,
        0,
        0,
        6,
        1,
        4,
        0,
        0,
        1,
        5,
        0,
        0,
        1,
        8,
        5,
        2,
        0,
        0,
        6,
        1,
        4,
        0,
        0,
        1,
        5,
        0,
        0,
        2,
        4,
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
        3,
        2,
        0,
        0,
        4,
        1,
        4,
        0,
        0,
        1,
        5,
        0,
        0,
        1,
        8,
        5,
        2,
        0,
        0,
        6,
        1,
        4,
        0,
        0,
        1,
        5,
        0,
        0,
        0,
        6,
        1,
        3,
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
    value :: for i := 0; i < 1; i += 1 {
        break i
    } else {
        break 99
    }
    prn($"{value}")
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
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
        3,
        2,
        0,
        0,
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
        1,
        3,
        0,
        0,
        3,
        1,
        0,
        0,
        0,
        4,
        1,
        3,
        0,
        0,
        3,
        1,
        0,
        0,
        0,
        2,
        2,
        4,
        0,
        0,
        3,
        1,
        3,
        0,
        1,
        8,
        5,
        2,
        0,
        0,
        6,
        1,
        0,
        0,
        1,
        6,
        4,
        2,
        0,
        1,
        8,
        5,
        2,
        0,
        0,
        6,
        2,
        3,
        0,
        2,
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
        5,
        0,
        0,
        0,
        6,
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

Point :: plex #c {
    x    i32
    y    i32
    name string
}

Packed :: plex #packed {
    a u8
    b i32
}

main :: fn () {
    p := Point { x: 1, y: 2, name: "first" }
    q := p with { y: 7, name: "second" }
    pp := ^q
    prn($"{pp.name} {pp.x} {pp.y}")
    _packed := Packed { a: 1, b: 2 }
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
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
        6,
        1,
        0,
        0,
        1,
        4,
        1,
        0,
        0,
        0,
        5,
        3,
        0,
        0,
        1,
        4,
        1,
        0,
        0,
        0,
        5,
        3,
        0,
        0,
        1,
        4,
        4,
        0,
        0,
        0,
        5,
        6,
        0,
        0,
        3,
        0,
        6,
        0,
        0,
        0,
        7,
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
        6,
        6,
        0,
        0,
        1,
        4,
        1,
        0,
        0,
        0,
        2,
        2,
        0,
        0,
        1,
        4,
        1,
        0,
        0,
        0,
        2,
        3,
        0,
        0,
        3,
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
        2,
        7,
        5,
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
        1,
        0,
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
        8,
        5,
        0,
        1,
        4,
        2,
        0,
        0,
        0,
        3,
        1,
        4,
        0,
        0,
        4,
        1,
        0,
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
        2,
        0,
        0,
        0,
        2,
        1,
        4,
        0,
        0,
        1,
        4,
        0,
        0,
        0,
        5,
        1,
        5,
        0,
        0,
        2,
        2,
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
        0,
        0,
        0,
        2,
        1,
        5,
        0,
        0,
        2,
        2,
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
        0,
        0,
        0,
        2,
        1,
        5,
        0,
        1,
        4,
        7,
        0,
        0,
        0,
        8,
        1,
        4,
        0,
        0,
        3,
        6,
        0,
        0,
        0,
        9,
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
    "method": "textDocument/semanticTokens/full",
    "params": {
      "textDocument": {
        "uri": "file:///home/matt/nerd/tests/mods/test/parts/body.n"
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
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": {
      "data": [
        0,
        0,
        3,
        2,
        0,
        0,
        4,
        10,
        1,
        0,
        0,
        11,
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
        5,
        0,
        0,
        0,
        5,
        1,
        4,
        0,
        0,
        2,
        3,
        0,
        0,
        0,
        5,
        2,
        4,
        0,
        0,
        3,
        5,
        0,
        0,
        1,
        4,
        6,
        2,
        0,
        0,
        7,
        5,
        0,
        0,
        0,
        8,
        5,
        0,
        0,
        0,
        5,
        1,
        4,
        0,
        0,
        2,
        5,
        0,
        0,
        3,
        0,
        3,
        2,
        0,
        0,
        4,
        11,
        1,
        0,
        0,
        12,
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
        6,
        2,
        4,
        0,
        0,
        3,
        3,
        0,
        0,
        1,
        4,
        5,
        0,
        0,
        0,
        6,
        1,
        4,
        0,
        0,
        3,
        10,
        1,
        0,
        0,
        11,
        2,
        3,
        0,
        1,
        4,
        6,
        2,
        0,
        0,
        7,
        5,
        0,
        0,
        0,
        5,
        1,
        4,
        0,
        0,
        1,
        5,
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
use std.io

main :: fn () => 0
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
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
        6,
        2,
        4,
        0,
        0,
        3,
        1,
        3,
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
acquire :: fn () -> ?void {
}

main :: fn () -> ?void {
    acquire()?
    undo prn("first")
    undo {
        prn("second")
    }
    return
}

¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
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
    "result": {
      "data": [
        0,
        0,
        7,
        1,
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
        2,
        2,
        0,
        0,
        6,
        2,
        4,
        0,
        0,
        4,
        4,
        0,
        0,
        3,
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
        0,
        6,
        2,
        4,
        0,
        0,
        4,
        4,
        0,
        0,
        1,
        4,
        7,
        1,
        0,
        1,
        4,
        4,
        2,
        0,
        0,
        5,
        3,
        1,
        0,
        0,
        4,
        7,
        5,
        0,
        1,
        4,
        4,
        2,
        0,
        1,
        8,
        3,
        1,
        0,
        0,
        4,
        8,
        5,
        0,
        2,
        4,
        6,
        2,
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
