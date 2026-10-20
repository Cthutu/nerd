answer :: 42

main :: fn () => answer
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
        "name": "answer",
        "kind": 14,
        "range": {
          "start": {
            "line": 0,
            "character": 0
          },
          "end": {
            "line": 0,
            "character": 6
          }
        },
        "selectionRange": {
          "start": {
            "line": 0,
            "character": 0
          },
          "end": {
            "line": 0,
            "character": 6
          }
        },
        "detail": "constant = 42"
      },
      {
        "name": "main",
        "kind": 12,
        "range": {
          "start": {
            "line": 2,
            "character": 0
          },
          "end": {
            "line": 2,
            "character": 4
          }
        },
        "selectionRange": {
          "start": {
            "line": 2,
            "character": 0
          },
          "end": {
            "line": 2,
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
        "name": "Frame",
        "kind": 14,
        "range": {
          "start": {
            "line": 0,
            "character": 0
          },
          "end": {
            "line": 0,
            "character": 5
          }
        },
        "selectionRange": {
          "start": {
            "line": 0,
            "character": 0
          },
          "end": {
            "line": 0,
            "character": 5
          }
        },
        "detail": "type alias"
      },
      {
        "name": "FrameSystem",
        "kind": 14,
        "range": {
          "start": {
            "line": 5,
            "character": 0
          },
          "end": {
            "line": 5,
            "character": 11
          }
        },
        "selectionRange": {
          "start": {
            "line": 5,
            "character": 0
          },
          "end": {
            "line": 5,
            "character": 11
          }
        },
        "detail": "type alias"
      },
      {
        "name": "done",
        "kind": 12,
        "range": {
          "start": {
            "line": 9,
            "character": 0
          },
          "end": {
            "line": 9,
            "character": 4
          }
        },
        "selectionRange": {
          "start": {
            "line": 9,
            "character": 0
          },
          "end": {
            "line": 9,
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
counter := 0
answer :: 42
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
        "name": "counter",
        "kind": 14,
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
        "detail": "function"
      },
      {
        "name": "answer",
        "kind": 14,
        "range": {
          "start": {
            "line": 1,
            "character": 0
          },
          "end": {
            "line": 1,
            "character": 6
          }
        },
        "selectionRange": {
          "start": {
            "line": 1,
            "character": 0
          },
          "end": {
            "line": 1,
            "character": 6
          }
        },
        "detail": "constant = 42"
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
on "debug" {
    answer :: 7

    ffi "c" {
        pub abs (value: i32) -> i32
    }
}

main :: fn () => answer
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
        "name": "answer",
        "kind": 14,
        "range": {
          "start": {
            "line": 1,
            "character": 4
          },
          "end": {
            "line": 1,
            "character": 10
          }
        },
        "selectionRange": {
          "start": {
            "line": 1,
            "character": 4
          },
          "end": {
            "line": 1,
            "character": 10
          }
        },
        "detail": "constant = 7"
      },
      {
        "name": "abs",
        "kind": 12,
        "range": {
          "start": {
            "line": 4,
            "character": 12
          },
          "end": {
            "line": 4,
            "character": 15
          }
        },
        "selectionRange": {
          "start": {
            "line": 4,
            "character": 12
          },
          "end": {
            "line": 4,
            "character": 15
          }
        },
        "detail": "function"
      },
      {
        "name": "main",
        "kind": 12,
        "range": {
          "start": {
            "line": 8,
            "character": 0
          },
          "end": {
            "line": 8,
            "character": 4
          }
        },
        "selectionRange": {
          "start": {
            "line": 8,
            "character": 0
          },
          "end": {
            "line": 8,
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
use std.io

answer :: 42

main :: fn () => answer
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
    "result": [
      {
        "name": "answer",
        "kind": 14,
        "range": {
          "start": {
            "line": 2,
            "character": 0
          },
          "end": {
            "line": 2,
            "character": 6
          }
        },
        "selectionRange": {
          "start": {
            "line": 2,
            "character": 0
          },
          "end": {
            "line": 2,
            "character": 6
          }
        },
        "detail": "constant = 42"
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
Display :: trait {
    show :: fn (self: Self) -> string
}

main :: fn () => 0
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
    "id": 3,
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
