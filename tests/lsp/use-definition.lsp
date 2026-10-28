use std.time
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
        "line": 0,
        "character": 3
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
        "character": 7
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
        "line": 0,
        "character": 8
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
        "line": 0,
        "character": 11
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
        "line": 0,
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
              "character": 12
            }
          },
          "severity": 4,
          "source": "nerd",
          "message": "Unused use `std.time`",
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
      "uri": "__REPO_URI__/mods/std/time.n",
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
      "uri": "__REPO_URI__/mods/std/time.n",
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
      "uri": "__REPO_URI__/mods/std/time.n",
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
      "uri": "__REPO_URI__/mods/std/time.n",
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
      "uri": "__REPO_URI__/mods/std/time.n",
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
      "uri": "__REPO_URI__/mods/std/time.n",
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
      "uri": "__REPO_URI__/mods/std/time.n",
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
use test.broken_import_exports
use std.time
alias :: use std.time
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
        "line": 1,
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
        "line": 2,
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
      "uri": "__REPO_URI__/mods/std/time.n",
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
      "uri": "__REPO_URI__/mods/std/time.n",
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
      "uri": "__REPO_URI__/mods/std/time.n",
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
use std { io time }
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
        "line": 0,
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
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 0,
              "character": 10
            },
            "end": {
              "line": 0,
              "character": 12
            }
          },
          "severity": 4,
          "source": "nerd",
          "message": "Unused use `io`",
          "tags": [
            1
          ]
        },
        {
          "range": {
            "start": {
              "line": 0,
              "character": 13
            },
            "end": {
              "line": 0,
              "character": 17
            }
          },
          "severity": 4,
          "source": "nerd",
          "message": "Unused use `time`",
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
    "id": 3,
    "result": {
      "uri": "__REPO_URI__/mods/std/time.n",
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
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]
