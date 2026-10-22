Other :: plex { word u16 }
Hash :: plex {
    union {
        bytes [8]u8
        plex {
            word u32 -- First word in the hash.
            next u32
        }
    }
}
main :: fn () {
    hash: Hash
    hash.word = 42
    prn($"{hash.word}")
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
        "character": 9
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
        "character": 10
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
        "label": "bytes",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "word",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "next",
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
        "value": "```nerd\nword\n```\n\n- Kind: plex field\n- Type: `u32`\n- Owner: `Hash`\n\nFirst word in the hash."
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
          "line": 5,
          "character": 12
        },
        "end": {
          "line": 5,
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
use test.anonymous_records
main :: fn () {
    hash: Hash
    hash.a = 42
    prn($"{hash.a}")
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
        "character": 9
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
    "result": [
      {
        "label": "bytes",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "a",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "b",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "c",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "d",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "e",
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
        "value": "```nerd\na\n```\n\n- Kind: plex field\n- Type: `u32`\n- Owner: `Hash`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "uri": "__REPO_URI__/tests/mods/test/anonymous_records.n",
      "range": {
        "start": {
          "line": 3,
          "character": 15
        },
        "end": {
          "line": 3,
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
Other :: plex { word u16 }
Hash :: plex {
    union {
        bytes [8]u8
        plex {
            word u32 -- First word in the hash.
            next u32
        }
    }
}
main :: fn () {
    hash: Hash
    hash.
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
              "line": 13,
              "character": 0
            },
            "end": {
              "line": 13,
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
                    "line": 13,
                    "character": 0
                  },
                  "end": {
                    "line": 13,
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
        "label": "bytes",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "word",
        "kind": 5,
        "detail": "field"
      },
      {
        "label": "next",
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
