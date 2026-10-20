main :: fn () -> i32 {
    value := 1
    value = value + 1
    return value
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/prepareRename",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 2,
        "character": 12
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/rename",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 2,
        "character": 12
      },
      "newName": "count"
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
      "range": {
        "start": {
          "line": 2,
          "character": 12
        },
        "end": {
          "line": 2,
          "character": 17
        }
      },
      "placeholder": "value"
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "changes": {
        "file:///test.n": [
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
            "newText": "count"
          },
          {
            "range": {
              "start": {
                "line": 2,
                "character": 4
              },
              "end": {
                "line": 2,
                "character": 9
              }
            },
            "newText": "count"
          },
          {
            "range": {
              "start": {
                "line": 2,
                "character": 12
              },
              "end": {
                "line": 2,
                "character": 17
              }
            },
            "newText": "count"
          },
          {
            "range": {
              "start": {
                "line": 3,
                "character": 11
              },
              "end": {
                "line": 3,
                "character": 16
              }
            },
            "newText": "count"
          }
        ]
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
answer :: 41
counter : i32 = 1

main :: fn () -> i32 {
    counter = counter + answer
    answer
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/rename",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 0,
        "character": 2
      },
      "newName": "value"
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/rename",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 1,
        "character": 2
      },
      "newName": "total"
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
              "character": 4
            },
            "end": {
              "line": 5,
              "character": 10
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expression result of type `i32` is not used",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
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
              },
              "message": "note: Only `void` expressions can be used as standalone statements."
            },
            {
              "location": {
                "uri": "file:///test.n",
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
      "changes": {
        "file:///test.n": [
          {
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
            "newText": "value"
          },
          {
            "range": {
              "start": {
                "line": 4,
                "character": 24
              },
              "end": {
                "line": 4,
                "character": 30
              }
            },
            "newText": "value"
          },
          {
            "range": {
              "start": {
                "line": 5,
                "character": 4
              },
              "end": {
                "line": 5,
                "character": 10
              }
            },
            "newText": "value"
          }
        ]
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "changes": {
        "file:///test.n": [
          {
            "range": {
              "start": {
                "line": 1,
                "character": 0
              },
              "end": {
                "line": 1,
                "character": 7
              }
            },
            "newText": "total"
          },
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
            "newText": "total"
          },
          {
            "range": {
              "start": {
                "line": 4,
                "character": 14
              },
              "end": {
                "line": 4,
                "character": 21
              }
            },
            "newText": "total"
          }
        ]
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
main :: fn () {}
¬
[
  {
    "jsonrpc": "2.0",
    "method": "textDocument/didOpen",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/086-rename-with-import-error/main.n",
        "languageId": "nerd",
        "version": 1,
        "text": "bad :: use bad\n\nmain :: fn () -> i32 {\n    _value := 41\n    return _value\n}\n"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/prepareRename",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/086-rename-with-import-error/main.n"
      },
      "position": {
        "line": 3,
        "character": 5
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/rename",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/086-rename-with-import-error/main.n"
      },
      "position": {
        "line": 4,
        "character": 12
      },
      "newName": "value"
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
      "uri": "__REPO_URI__/tests/lsp/086-rename-with-import-error/main.n",
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "method": "textDocument/publishDiagnostics",
    "params": {
      "uri": "__REPO_URI__/tests/lsp/086-rename-with-import-error/bad/mod.n",
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
                "uri": "__REPO_URI__/tests/lsp/086-rename-with-import-error/bad/mod.n",
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
    "id": 2,
    "result": {
      "range": {
        "start": {
          "line": 3,
          "character": 4
        },
        "end": {
          "line": 3,
          "character": 10
        }
      },
      "placeholder": "_value"
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "changes": {
        "__REPO_URI__/tests/lsp/086-rename-with-import-error/main.n": [
          {
            "range": {
              "start": {
                "line": 3,
                "character": 4
              },
              "end": {
                "line": 3,
                "character": 10
              }
            },
            "newText": "value"
          },
          {
            "range": {
              "start": {
                "line": 4,
                "character": 11
              },
              "end": {
                "line": 4,
                "character": 17
              }
            },
            "newText": "value"
          }
        ]
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
    "method": "textDocument/prepareRename",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 10,
        "character": 10
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/rename",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 10,
        "character": 10
      },
      "newName": "item"
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
      "range": {
        "start": {
          "line": 10,
          "character": 9
        },
        "end": {
          "line": 10,
          "character": 14
        }
      },
      "placeholder": "frame"
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "changes": {
        "file:///test.n": [
          {
            "range": {
              "start": {
                "line": 9,
                "character": 12
              },
              "end": {
                "line": 9,
                "character": 17
              }
            },
            "newText": "item"
          },
          {
            "range": {
              "start": {
                "line": 10,
                "character": 9
              },
              "end": {
                "line": 10,
                "character": 14
              }
            },
            "newText": "item"
          }
        ]
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
-- lsp-uri: __REPO_URI__/tests/lsp/148-cross-module-references-rename/main.n
-- lsp-root-uri: __REPO_URI__/tests/lsp/148-cross-module-references-rename
use helper

main :: fn () -> i32 {
    return answer + twice()
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/references",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/148-cross-module-references-rename/main.n"
      },
      "position": {
        "line": 3,
        "character": 11
      },
      "context": {
        "includeDeclaration": true
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/references",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/148-cross-module-references-rename/main.n"
      },
      "position": {
        "line": 3,
        "character": 11
      },
      "context": {
        "includeDeclaration": false
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "method": "textDocument/rename",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/148-cross-module-references-rename/main.n"
      },
      "position": {
        "line": 3,
        "character": 11
      },
      "newName": "count"
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
      "uri": "__REPO_URI__/tests/lsp/148-cross-module-references-rename/main.n",
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": [
      {
        "uri": "__REPO_URI__/tests/lsp/148-cross-module-references-rename/main.n",
        "range": {
          "start": {
            "line": 3,
            "character": 11
          },
          "end": {
            "line": 3,
            "character": 17
          }
        }
      },
      {
        "uri": "__REPO_URI__/tests/lsp/148-cross-module-references-rename/helper.n",
        "range": {
          "start": {
            "line": 0,
            "character": 4
          },
          "end": {
            "line": 0,
            "character": 10
          }
        }
      },
      {
        "uri": "__REPO_URI__/tests/lsp/148-cross-module-references-rename/helper.n",
        "range": {
          "start": {
            "line": 3,
            "character": 11
          },
          "end": {
            "line": 3,
            "character": 17
          }
        }
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": [
      {
        "uri": "__REPO_URI__/tests/lsp/148-cross-module-references-rename/main.n",
        "range": {
          "start": {
            "line": 3,
            "character": 11
          },
          "end": {
            "line": 3,
            "character": 17
          }
        }
      },
      {
        "uri": "__REPO_URI__/tests/lsp/148-cross-module-references-rename/helper.n",
        "range": {
          "start": {
            "line": 3,
            "character": 11
          },
          "end": {
            "line": 3,
            "character": 17
          }
        }
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "changes": {
        "__REPO_URI__/tests/lsp/148-cross-module-references-rename/main.n": [
          {
            "range": {
              "start": {
                "line": 3,
                "character": 11
              },
              "end": {
                "line": 3,
                "character": 17
              }
            },
            "newText": "count"
          }
        ],
        "__REPO_URI__/tests/lsp/148-cross-module-references-rename/helper.n": [
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
            "newText": "count"
          },
          {
            "range": {
              "start": {
                "line": 3,
                "character": 11
              },
              "end": {
                "line": 3,
                "character": 17
              }
            },
            "newText": "count"
          }
        ]
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]
