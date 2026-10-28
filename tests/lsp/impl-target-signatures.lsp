use test.impl_target_types
Factory :: plex { value i32 }
impl Factory {
    make :: fn (value: i32) -> Factory { return { value } }
    read :: fn (self: Factory) -> i32 { return self.value }
}
main :: fn () {
    value := Factory.make(42)
    assert value.read() == 42
    imported := ShapeAlias.make(7)
    assert imported.get() == 7
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
        "character": 21
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
        "line": 8,
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
        "line": 9,
        "character": 27
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
        "line": 7,
        "character": 21
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
        "line": 9,
        "character": 27
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
        "value": "```nerd\nmake :: fn (value: i32) -> Factory\n```\n\n- Kind: method"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nread :: fn (self: Factory) -> i32\n```\n\n- Kind: method"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nmake :: fn (value: i32) -> Shape\n```\n\n- Kind: method"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "result": {
      "uri": "file:///test.n",
      "range": {
        "start": {
          "line": 3,
          "character": 4
        },
        "end": {
          "line": 3,
          "character": 8
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "result": {
      "uri": "__REPO_URI__/tests/mods/test/impl_target_types.n",
      "range": {
        "start": {
          "line": 4,
          "character": 8
        },
        "end": {
          "line": 4,
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
Thing :: plex { value i32 }
impl Thing {
    unrelated :: fn () -> i32 { return 1 }
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
              "character": 4
            },
            "end": {
              "line": 2,
              "character": 13
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Impl function `unrelated` has no receiver or return type related to the impl target",
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
                    "character": 13
                  }
                }
              },
              "message": "help: Use `Self` or the explicit target type for a receiver or factory return type, or move this function outside the impl block."
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
