Point :: plex {
    x i32
    y i32
    visible bool
    name string
}

main :: fn () {
    point := Point {
        x: 1
    }
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 8,
          "character": 13
        },
        "end": {
          "line": 8,
          "character": 13
        }
      },
      "context": {
        "diagnostics": []
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
              "character": 19
            },
            "end": {
              "line": 8,
              "character": 20
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
                    "character": 19
                  },
                  "end": {
                    "line": 8,
                    "character": 20
                  }
                }
              },
              "message": "note: Missing fields: `y`, `visible`, `name`"
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 8,
                    "character": 19
                  },
                  "end": {
                    "line": 8,
                    "character": 20
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
    "id": 2,
    "result": [
      {
        "title": "Fill missing plex fields",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 10,
                    "character": 4
                  },
                  "end": {
                    "line": 10,
                    "character": 4
                  }
                },
                "newText": "\n        y      : 0\n        visible: no\n        name   : \"\"\n    "
              }
            ]
          }
        }
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
types :: use test.lsp_types

main :: fn () -> i32 {
    point := types.Point {
        x: 1
    }
    return point.x
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 3,
          "character": 21
        },
        "end": {
          "line": 3,
          "character": 21
        }
      },
      "context": {
        "diagnostics": []
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
              "character": 25
            },
            "end": {
              "line": 3,
              "character": 26
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Plex literal is missing required field",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 3,
                    "character": 25
                  },
                  "end": {
                    "line": 3,
                    "character": 26
                  }
                }
              },
              "message": "note: Missing field: `y`"
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 3,
                    "character": 25
                  },
                  "end": {
                    "line": 3,
                    "character": 26
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
    "id": 2,
    "result": [
      {
        "title": "Fill missing plex fields",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 5,
                    "character": 4
                  },
                  "end": {
                    "line": 5,
                    "character": 4
                  }
                },
                "newText": "\n        y: 0\n    "
              }
            ]
          }
        }
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
    main_frame: Frame = { }
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 3,
          "character": 25
        },
        "end": {
          "line": 3,
          "character": 25
        }
      },
      "context": {
        "diagnostics": []
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
              "character": 24
            },
            "end": {
              "line": 3,
              "character": 25
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
                    "line": 3,
                    "character": 24
                  },
                  "end": {
                    "line": 3,
                    "character": 25
                  }
                }
              },
              "message": "note: Missing fields: `system`, `id`, `width`, `height`, `title`, `full_screen`, `resizable`"
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 3,
                    "character": 24
                  },
                  "end": {
                    "line": 3,
                    "character": 25
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
    "id": 2,
    "result": [
      {
        "title": "Fill missing plex fields",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 3,
                    "character": 25
                  },
                  "end": {
                    "line": 3,
                    "character": 26
                  }
                },
                "newText": "\n        system     : nil\n        id         : 0\n        width      : 0\n        height     : 0\n        title      : \"\"\n        full_screen: no\n        resizable  : no\n    "
              }
            ]
          }
        }
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
    value := input()
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 1,
          "character": 14
        },
        "end": {
          "line": 1,
          "character": 14
        }
      },
      "context": {
        "diagnostics": []
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
              "character": 13
            },
            "end": {
              "line": 1,
              "character": 18
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown symbol `input`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 1,
                    "character": 13
                  },
                  "end": {
                    "line": 1,
                    "character": 18
                  }
                }
              },
              "message": "help: Add a binding for `input` or fix the spelling."
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
        "title": "Add use std.io",
        "kind": "quickfix",
        "edit": {
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
                    "character": 0
                  }
                },
                "newText": "use std.io\n\n"
              }
            ]
          }
        }
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
-- lsp-uri: __REPO_URI__/tests/lsp/121-local-add-use-code-action.input.n
main :: fn () {
    local_helper()
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/121-local-add-use-code-action.input.n"
      },
      "range": {
        "start": {
          "line": 1,
          "character": 8
        },
        "end": {
          "line": 1,
          "character": 8
        }
      },
      "context": {
        "diagnostics": []
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
      "uri": "__REPO_URI__/tests/lsp/121-local-add-use-code-action.input.n",
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 1,
              "character": 4
            },
            "end": {
              "line": 1,
              "character": 16
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown symbol `local_helper`",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/tests/lsp/121-local-add-use-code-action.input.n",
                "range": {
                  "start": {
                    "line": 1,
                    "character": 4
                  },
                  "end": {
                    "line": 1,
                    "character": 16
                  }
                }
              },
              "message": "help: Add a binding for `local_helper` or fix the spelling."
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
        "title": "Add use local_import_fixture",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "__REPO_URI__/tests/lsp/121-local-add-use-code-action.input.n": [
              {
                "range": {
                  "start": {
                    "line": 0,
                    "character": 0
                  },
                  "end": {
                    "line": 0,
                    "character": 0
                  }
                },
                "newText": "use local_import_fixture\n\n"
              }
            ]
          }
        }
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
memory :: use std.memory

main :: fn () {
    value := input()
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 3,
          "character": 14
        },
        "end": {
          "line": 3,
          "character": 14
        }
      },
      "context": {
        "diagnostics": []
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
              "character": 13
            },
            "end": {
              "line": 3,
              "character": 18
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown symbol `input`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 3,
                    "character": 13
                  },
                  "end": {
                    "line": 3,
                    "character": 18
                  }
                }
              },
              "message": "help: Add a binding for `input` or fix the spelling."
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
        "title": "Add use std.io",
        "kind": "quickfix",
        "edit": {
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
                    "character": 0
                  }
                },
                "newText": "use std.io\n"
              }
            ]
          }
        }
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
-- TurfWar
-- Windows shell

main :: fn () {
    value := input()
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 4,
          "character": 14
        },
        "end": {
          "line": 4,
          "character": 14
        }
      },
      "context": {
        "diagnostics": []
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
              "character": 13
            },
            "end": {
              "line": 4,
              "character": 18
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown symbol `input`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 4,
                    "character": 13
                  },
                  "end": {
                    "line": 4,
                    "character": 18
                  }
                }
              },
              "message": "help: Add a binding for `input` or fix the spelling."
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
        "title": "Add use std.io",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 3,
                    "character": 0
                  },
                  "end": {
                    "line": 3,
                    "character": 0
                  }
                },
                "newText": "use std.io\n\n"
              }
            ]
          }
        }
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
--
-- Demo
--

pragma windowed

use std.memory

main :: fn () {
    value := input()
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 9,
          "character": 14
        },
        "end": {
          "line": 9,
          "character": 14
        }
      },
      "context": {
        "diagnostics": []
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
              "line": 9,
              "character": 13
            },
            "end": {
              "line": 9,
              "character": 18
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown symbol `input`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 9,
                    "character": 13
                  },
                  "end": {
                    "line": 9,
                    "character": 18
                  }
                }
              },
              "message": "help: Add a binding for `input` or fix the spelling."
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
        "title": "Add use std.io",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 7,
                    "character": 0
                  },
                  "end": {
                    "line": 7,
                    "character": 0
                  }
                },
                "newText": "use std.io\n"
              }
            ]
          }
        }
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
-- lsp-uri: __REPO_URI__/tests/lsp/126-add-use-method-code-action.input.n
main :: fn () {
    text := "a b"
    parts := text.split_words(" ")
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/126-add-use-method-code-action.input.n"
      },
      "range": {
        "start": {
          "line": 2,
          "character": 20
        },
        "end": {
          "line": 2,
          "character": 20
        }
      },
      "context": {
        "diagnostics": []
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
      "uri": "__REPO_URI__/tests/lsp/126-add-use-method-code-action.input.n",
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 2,
              "character": 18
            },
            "end": {
              "line": 2,
              "character": 29
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Type mismatch: expected `string field .data, .count, .bytes, or defined method`, found `split_words`",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/tests/lsp/126-add-use-method-code-action.input.n",
                "range": {
                  "start": {
                    "line": 2,
                    "character": 18
                  },
                  "end": {
                    "line": 2,
                    "character": 29
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
        "title": "Add use local_string_methods",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "__REPO_URI__/tests/lsp/126-add-use-method-code-action.input.n": [
              {
                "range": {
                  "start": {
                    "line": 0,
                    "character": 0
                  },
                  "end": {
                    "line": 0,
                    "character": 0
                  }
                },
                "newText": "use local_string_methods\n\n"
              }
            ]
          }
        }
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
    value := 1
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
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
      "context": {
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
            "message": "Unused local variable `value`"
          }
        ]
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
    "id": 2,
    "result": [
      {
        "title": "Rename to _value",
        "kind": "quickfix",
        "edit": {
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
                "newText": "_value"
              }
            ]
          }
        }
      },
      {
        "title": "Remove value",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 1,
                    "character": 0
                  },
                  "end": {
                    "line": 2,
                    "character": 0
                  }
                },
                "newText": ""
              }
            ]
          }
        }
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
main :: fn () -> i32 {
    _result := 1
    return _result
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 2,
          "character": 11
        },
        "end": {
          "line": 2,
          "character": 18
        }
      },
      "context": {
        "diagnostics": [
          {
            "range": {
              "start": {
                "line": 2,
                "character": 11
              },
              "end": {
                "line": 2,
                "character": 18
              }
            },
            "severity": 1,
            "source": "nerd",
            "message": "Used local variable `_result` marked as unused"
          }
        ]
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
              "character": 11
            },
            "end": {
              "line": 2,
              "character": 18
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Used local variable `_result` marked as unused",
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
              "message": "`_result` is marked unused by its leading `_`"
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 2,
                    "character": 11
                  },
                  "end": {
                    "line": 2,
                    "character": 18
                  }
                }
              },
              "message": "note: Leading `_` names are reserved for bindings that are deliberately unused."
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 2,
                    "character": 11
                  },
                  "end": {
                    "line": 2,
                    "character": 18
                  }
                }
              },
              "message": "help: Rename `_result` without the leading `_` now that it is used."
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
        "title": "Rename to result",
        "kind": "quickfix",
        "edit": {
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
                    "character": 11
                  }
                },
                "newText": "result"
              },
              {
                "range": {
                  "start": {
                    "line": 2,
                    "character": 11
                  },
                  "end": {
                    "line": 2,
                    "character": 18
                  }
                },
                "newText": "result"
              }
            ]
          }
        }
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
    show :: fn (self: ^Self) -> string
    reset :: fn (self: Self)
}

Point :: plex {
    x i32
}

impl Display for Point {
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 10,
          "character": 0
        },
        "end": {
          "line": 10,
          "character": 0
        }
      },
      "context": {
        "diagnostics": []
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
              "line": 9,
              "character": 0
            },
            "end": {
              "line": 9,
              "character": 4
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Trait implementation is missing required members",
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
                    "character": 7
                  }
                }
              },
              "message": "Type `Display` is defined here"
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 9,
                    "character": 0
                  },
                  "end": {
                    "line": 9,
                    "character": 4
                  }
                }
              },
              "message": "note: Missing members: `show`, `reset`"
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 9,
                    "character": 0
                  },
                  "end": {
                    "line": 9,
                    "character": 4
                  }
                }
              },
              "message": "help: Add the missing members to this `impl` block."
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
        "title": "Stub missing trait members",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 10,
                    "character": 0
                  },
                  "end": {
                    "line": 10,
                    "character": 0
                  }
                },
                "newText": "    show :: fn (_self: ^Self) -> string {\n        result: string\n        return result\n    }\n\n    reset :: fn (_self: Self) {\n    }\n"
              }
            ]
          }
        }
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
-- lsp-uri: __REPO_URI__/tests/lsp/130-workspace-root-add-use/tiny_engine/graphics.n
-- lsp-root-uri: __REPO_URI__/tests/lsp/130-workspace-root-add-use
Graphics :: plex {
    wnd HWND
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/130-workspace-root-add-use/tiny_engine/graphics.n"
      },
      "range": {
        "start": {
          "line": 1,
          "character": 8
        },
        "end": {
          "line": 1,
          "character": 12
        }
      },
      "context": {
        "diagnostics": []
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
      "uri": "__REPO_URI__/tests/lsp/130-workspace-root-add-use/tiny_engine/graphics.n",
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 1,
              "character": 8
            },
            "end": {
              "line": 1,
              "character": 12
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown type `HWND`",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/tests/lsp/130-workspace-root-add-use/tiny_engine/graphics.n",
                "range": {
                  "start": {
                    "line": 1,
                    "character": 8
                  },
                  "end": {
                    "line": 1,
                    "character": 12
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
    "id": 2,
    "result": [
      {
        "title": "Add use test.lsp_windows_exports",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "__REPO_URI__/tests/lsp/130-workspace-root-add-use/tiny_engine/graphics.n": [
              {
                "range": {
                  "start": {
                    "line": 0,
                    "character": 0
                  },
                  "end": {
                    "line": 0,
                    "character": 0
                  }
                },
                "newText": "use test.lsp_windows_exports\n\n"
              }
            ]
          }
        }
      },
      {
        "title": "Add use os.windows",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "__REPO_URI__/tests/lsp/130-workspace-root-add-use/tiny_engine/graphics.n": [
              {
                "range": {
                  "start": {
                    "line": 0,
                    "character": 0
                  },
                  "end": {
                    "line": 0,
                    "character": 0
                  }
                },
                "newText": "use os.windows\n\n"
              }
            ]
          }
        }
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
-- lsp-uri: __REPO_URI__/tests/lsp/131-open-module-reanalysis/bar.n
-- lsp-root-uri: __REPO_URI__/tests/lsp/131-open-module-reanalysis
use foo

value :: fn (_arg: Foo) {}
¬
[
  {
    "jsonrpc": "2.0",
    "method": "textDocument/didOpen",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/131-open-module-reanalysis/foo.n",
        "languageId": "nerd",
        "version": 1,
        "text": "pub Foo :: usize\n"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/131-open-module-reanalysis/bar.n"
      },
      "range": {
        "start": {
          "line": 2,
          "character": 19
        },
        "end": {
          "line": 2,
          "character": 22
        }
      },
      "context": {
        "diagnostics": []
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
      "uri": "__REPO_URI__/tests/lsp/131-open-module-reanalysis/bar.n",
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 2,
              "character": 19
            },
            "end": {
              "line": 2,
              "character": 22
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown type `Foo`",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/tests/lsp/131-open-module-reanalysis/bar.n",
                "range": {
                  "start": {
                    "line": 2,
                    "character": 19
                  },
                  "end": {
                    "line": 2,
                    "character": 22
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
    "method": "textDocument/publishDiagnostics",
    "params": {
      "uri": "__REPO_URI__/tests/lsp/131-open-module-reanalysis/foo.n",
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "method": "textDocument/publishDiagnostics",
    "params": {
      "uri": "__REPO_URI__/tests/lsp/131-open-module-reanalysis/bar.n",
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
Counter :: plex {
    window_count i32
}

main :: fn () {
    c := Counter { window_count: 1 }
    _ := c.windows_count
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 6,
          "character": 11
        },
        "end": {
          "line": 6,
          "character": 24
        }
      },
      "context": {
        "diagnostics": [
          {
            "range": {
              "start": {
                "line": 6,
                "character": 11
              },
              "end": {
                "line": 6,
                "character": 24
              }
            },
            "severity": 1,
            "source": "nerd",
            "message": "Unknown member `windows_count` for `Counter`",
            "relatedInformation": [
              {
                "location": {
                  "uri": "file:///test.n",
                  "range": {
                    "start": {
                      "line": 6,
                      "character": 11
                    },
                    "end": {
                      "line": 6,
                      "character": 24
                    }
                  }
                },
                "message": "help: Did you mean `.window_count`?"
              }
            ]
          }
        ]
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
              "character": 11
            },
            "end": {
              "line": 6,
              "character": 24
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown member `windows_count` for `Counter`",
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
                    "character": 7
                  }
                }
              },
              "message": "Type `Counter` is defined here"
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 6,
                    "character": 11
                  },
                  "end": {
                    "line": 6,
                    "character": 24
                  }
                }
              },
              "message": "help: Did you mean `.window_count`?"
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
        "title": "Change to window_count",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 6,
                    "character": 11
                  },
                  "end": {
                    "line": 6,
                    "character": 24
                  }
                },
                "newText": "window_count"
              }
            ]
          }
        }
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
-- lsp-uri: __REPO_URI__/examples/text-adventure/quill.n
-- lsp-root-uri: __REPO_URI__
main :: fn () {
    parts := "a b".split(" ")
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/examples/text-adventure/quill.n"
      },
      "range": {
        "start": {
          "line": 1,
          "character": 19
        },
        "end": {
          "line": 1,
          "character": 19
        }
      },
      "context": {
        "diagnostics": []
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
      "uri": "__REPO_URI__/examples/text-adventure/quill.n",
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 1,
              "character": 19
            },
            "end": {
              "line": 1,
              "character": 24
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Type mismatch: expected `string field .data, .count, .bytes, or defined method`, found `split`",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/examples/text-adventure/quill.n",
                "range": {
                  "start": {
                    "line": 1,
                    "character": 19
                  },
                  "end": {
                    "line": 1,
                    "character": 24
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
        "title": "Add use std.text",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "__REPO_URI__/examples/text-adventure/quill.n": [
              {
                "range": {
                  "start": {
                    "line": 0,
                    "character": 0
                  },
                  "end": {
                    "line": 0,
                    "character": 0
                  }
                },
                "newText": "use std.text\n\n"
              }
            ]
          }
        }
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
-- lsp-uri: file:///tmp/nerd-env-code-action/main.n
-- lsp-root-uri: file:///tmp/nerd-env-code-action
-- lsp-lib-path: __REPO_URI__/tests/lsp/env_lib
main :: fn () {
    env_only_helper()
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///tmp/nerd-env-code-action/main.n"
      },
      "range": {
        "start": {
          "line": 1,
          "character": 8
        },
        "end": {
          "line": 1,
          "character": 8
        }
      },
      "context": {
        "diagnostics": []
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
      "uri": "file:///tmp/nerd-env-code-action/main.n",
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 1,
              "character": 4
            },
            "end": {
              "line": 1,
              "character": 19
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown symbol `env_only_helper`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///tmp/nerd-env-code-action/main.n",
                "range": {
                  "start": {
                    "line": 1,
                    "character": 4
                  },
                  "end": {
                    "line": 1,
                    "character": 19
                  }
                }
              },
              "message": "help: Add a binding for `env_only_helper` or fix the spelling."
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
        "title": "Add use envonly",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///tmp/nerd-env-code-action/main.n": [
              {
                "range": {
                  "start": {
                    "line": 0,
                    "character": 0
                  },
                  "end": {
                    "line": 0,
                    "character": 0
                  }
                },
                "newText": "use envonly\n\n"
              }
            ]
          }
        }
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
    map := 1
    use_map(map)
}

simulate :: fn () {
    map := 2
    use_map(map)
}

use_map :: fn (value: i32) {
    on value < 0 => return
}

generate_map :: fn (map: i32) {
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 14,
          "character": 20
        },
        "end": {
          "line": 14,
          "character": 23
        }
      },
      "context": {
        "diagnostics": [
          {
            "range": {
              "start": {
                "line": 14,
                "character": 20
              },
              "end": {
                "line": 14,
                "character": 23
              }
            },
            "severity": 1,
            "source": "nerd",
            "message": "Unused parameter `map`"
          }
        ]
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
              "character": 20
            },
            "end": {
              "line": 14,
              "character": 23
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unused parameter `map`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 14,
                    "character": 20
                  },
                  "end": {
                    "line": 14,
                    "character": 23
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
                    "line": 14,
                    "character": 20
                  },
                  "end": {
                    "line": 14,
                    "character": 23
                  }
                }
              },
              "message": "help: Remove `map` or prefix the name with `_` if it is deliberately unused."
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
        "title": "Rename to _map",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 14,
                    "character": 20
                  },
                  "end": {
                    "line": 14,
                    "character": 23
                  }
                },
                "newText": "_map"
              }
            ]
          }
        }
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
use std.gfx

main :: fn () {
    mode := PixelLayerMode.FixedSizeAutoScale {
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
        "line": 3,
        "character": 27
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 4,
          "character": 4
        },
        "end": {
          "line": 4,
          "character": 4
        }
      },
      "context": {
        "diagnostics": []
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
              "character": 46
            },
            "end": {
              "line": 3,
              "character": 47
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
                    "line": 3,
                    "character": 46
                  },
                  "end": {
                    "line": 3,
                    "character": 47
                  }
                }
              },
              "message": "note: Missing fields: `width`, `height`"
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 3,
                    "character": 46
                  },
                  "end": {
                    "line": 3,
                    "character": 47
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
    "id": 2,
    "result": [
      {
        "label": "FitToWindow",
        "kind": 20
      },
      {
        "label": "FixedSizeAutoScale",
        "kind": 20
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": [
      {
        "title": "Fill missing plex fields",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 3,
                    "character": 47
                  },
                  "end": {
                    "line": 4,
                    "character": 4
                  }
                },
                "newText": "\n        width : 0\n        height: 0\n    "
              }
            ]
          }
        }
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
-- lsp-uri: __REPO_URI__/tests/lsp/144-add-use-skips-core-method.input.n
main :: fn () {
    alloc()
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/144-add-use-skips-core-method.input.n"
      },
      "range": {
        "start": {
          "line": 1,
          "character": 5
        },
        "end": {
          "line": 1,
          "character": 5
        }
      },
      "context": {
        "diagnostics": []
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
      "uri": "__REPO_URI__/tests/lsp/144-add-use-skips-core-method.input.n",
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
          "message": "Unknown symbol `alloc`",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/tests/lsp/144-add-use-skips-core-method.input.n",
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
              "message": "help: Add a binding for `alloc` or fix the spelling."
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
        "title": "Add use std.memory",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "__REPO_URI__/tests/lsp/144-add-use-skips-core-method.input.n": [
              {
                "range": {
                  "start": {
                    "line": 0,
                    "character": 0
                  },
                  "end": {
                    "line": 0,
                    "character": 0
                  }
                },
                "newText": "use std.memory\n\n"
              }
            ]
          }
        }
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
bad : fn () -> i32 {
    return 1
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 0,
          "character": 4
        },
        "end": {
          "line": 0,
          "character": 5
        }
      },
      "context": {
        "diagnostics": [
          {
            "range": {
              "start": {
                "line": 0,
                "character": 19
              },
              "end": {
                "line": 0,
                "character": 20
              }
            },
            "severity": 1,
            "source": "nerd",
            "message": "Expected Equal `=` but found LeftBrace `{`"
          }
        ]
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
              "character": 19
            },
            "end": {
              "line": 0,
              "character": 20
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected Equal `=` but found LeftBrace `{`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 0,
                    "character": 19
                  },
                  "end": {
                    "line": 0,
                    "character": 20
                  }
                }
              },
              "message": "note: A function type annotation cannot include a function body."
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 0,
                    "character": 19
                  },
                  "end": {
                    "line": 0,
                    "character": 20
                  }
                }
              },
              "message": "help: Function definitions use `::`; did you mean to write `::` instead of `:`?"
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
        "title": "Change `:` to `::`",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 0,
                    "character": 4
                  },
                  "end": {
                    "line": 0,
                    "character": 5
                  }
                },
                "newText": "::"
              }
            ]
          }
        }
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
    system := FrameSystem.init()
    on system.poll() {
    }
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 4,
          "character": 10
        },
        "end": {
          "line": 4,
          "character": 10
        }
      },
      "context": {
        "diagnostics": []
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
              "character": 18
            },
            "end": {
              "line": 4,
              "character": 19
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Argument count mismatch: expected 1, found 0",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 4,
                    "character": 18
                  },
                  "end": {
                    "line": 4,
                    "character": 19
                  }
                }
              },
              "message": "help: Pass an argument for parameter `frame`."
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
        "title": "Add missing enum variants",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 5,
                    "character": 4
                  },
                  "end": {
                    "line": 5,
                    "character": 4
                  }
                },
                "newText": "\n        None => {\n        }\n\n        Closed => {\n        }\n\n        Resized { width: _, height: _ } => {\n        }\n\n        KeyPress { scan_code: _ } => {\n        }\n\n        KeyRelease { scan_code: _ } => {\n        }\n\n        Character { codepoint: _ } => {\n        }\n    "
              }
            ]
          }
        }
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
-- lsp-uri: __REPO_URI__/tests/lsp/165-add-use-local-type-code-action/hello.n
main :: fn () {
    value : VAO
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/165-add-use-local-type-code-action/hello.n"
      },
      "range": {
        "start": {
          "line": 1,
          "character": 12
        },
        "end": {
          "line": 1,
          "character": 15
        }
      },
      "context": {
        "diagnostics": []
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
      "uri": "__REPO_URI__/tests/lsp/165-add-use-local-type-code-action/hello.n",
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 1,
              "character": 12
            },
            "end": {
              "line": 1,
              "character": 15
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown type `VAO`",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/tests/lsp/165-add-use-local-type-code-action/hello.n",
                "range": {
                  "start": {
                    "line": 1,
                    "character": 12
                  },
                  "end": {
                    "line": 1,
                    "character": 15
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
    "id": 2,
    "result": [
      {
        "title": "Add use vao",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "__REPO_URI__/tests/lsp/165-add-use-local-type-code-action/hello.n": [
              {
                "range": {
                  "start": {
                    "line": 0,
                    "character": 0
                  },
                  "end": {
                    "line": 0,
                    "character": 0
                  }
                },
                "newText": "use vao\n\n"
              }
            ]
          }
        }
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
-- lsp-uri: __REPO_URI__/tests/lsp/166-source-root-add-use/src/hello.n
-- lsp-root-uri: __REPO_URI__/tests/lsp/166-source-root-add-use
main :: fn () {
    value : VAO
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/166-source-root-add-use/src/hello.n"
      },
      "range": {
        "start": {
          "line": 1,
          "character": 12
        },
        "end": {
          "line": 1,
          "character": 15
        }
      },
      "context": {
        "diagnostics": []
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
      "uri": "__REPO_URI__/tests/lsp/166-source-root-add-use/src/hello.n",
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 1,
              "character": 12
            },
            "end": {
              "line": 1,
              "character": 15
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unknown type `VAO`",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/tests/lsp/166-source-root-add-use/src/hello.n",
                "range": {
                  "start": {
                    "line": 1,
                    "character": 12
                  },
                  "end": {
                    "line": 1,
                    "character": 15
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
    "id": 2,
    "result": [
      {
        "title": "Add use vao",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "__REPO_URI__/tests/lsp/166-source-root-add-use/src/hello.n": [
              {
                "range": {
                  "start": {
                    "line": 0,
                    "character": 0
                  },
                  "end": {
                    "line": 0,
                    "character": 0
                  }
                },
                "newText": "use vao\n\n"
              }
            ]
          }
        }
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
    second_frame := Frame {}
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 3,
          "character": 27
        },
        "end": {
          "line": 3,
          "character": 27
        }
      },
      "context": {
        "diagnostics": []
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
              "character": 26
            },
            "end": {
              "line": 3,
              "character": 27
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
                    "line": 3,
                    "character": 26
                  },
                  "end": {
                    "line": 3,
                    "character": 27
                  }
                }
              },
              "message": "note: Missing fields: `system`, `id`, `width`, `height`, `title`, `full_screen`, `resizable`"
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 3,
                    "character": 26
                  },
                  "end": {
                    "line": 3,
                    "character": 27
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
    "id": 2,
    "result": [
      {
        "title": "Fill missing plex fields",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 3,
                    "character": 27
                  },
                  "end": {
                    "line": 3,
                    "character": 27
                  }
                },
                "newText": "\n        system     : nil\n        id         : 0\n        width      : 0\n        height     : 0\n        title      : \"\"\n        full_screen: no\n        resizable  : no\n    "
              }
            ]
          }
        }
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
        version : 4
        _       : 2
        kind TokenType : 2
    }
    length u16
}

main :: fn () {
    _ := Header {
        version: 1
    }
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 13,
          "character": 8
        },
        "end": {
          "line": 13,
          "character": 8
        }
      },
      "context": {
        "diagnostics": []
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
              "character": 16
            },
            "end": {
              "line": 12,
              "character": 17
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
                    "line": 12,
                    "character": 16
                  },
                  "end": {
                    "line": 12,
                    "character": 17
                  }
                }
              },
              "message": "note: Missing fields: `kind`, `length`"
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 12,
                    "character": 16
                  },
                  "end": {
                    "line": 12,
                    "character": 17
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
    "id": 2,
    "result": [
      {
        "title": "Fill missing plex fields",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 14,
                    "character": 4
                  },
                  "end": {
                    "line": 14,
                    "character": 4
                  }
                },
                "newText": "\n        kind   : Eof\n        length : 0\n    "
              }
            ]
          }
        }
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
    frame_system := FrameSystem.init()
    defer frame_system.done()

    main_window := {
    }

    frame_system.apply(^main_window)
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
        "line": 3,
        "character": 33
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
        "character": 24
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
        "line": 9,
        "character": 20
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 7,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 6,
          "character": 20
        },
        "end": {
          "line": 6,
          "character": 20
        }
      },
      "context": {
        "diagnostics": []
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
        "line": 9,
        "character": 27
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
        "line": 4,
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
        "version": 2
      },
      "contentChanges": [
        {
          "range": {
            "start": {
              "line": 7,
              "character": 4
            },
            "end": {
              "line": 7,
              "character": 4
            }
          },
          "text": "..."
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
        "line": 3,
        "character": 8
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
        "line": 3,
        "character": 33
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
        "line": 4,
        "character": 24
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
        "line": 6,
        "character": 8
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
        "line": 9,
        "character": 20
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 17,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 6,
          "character": 20
        },
        "end": {
          "line": 6,
          "character": 20
        }
      },
      "context": {
        "diagnostics": []
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 18,
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
    "id": 19,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 4,
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
        "version": 3
      },
      "contentChanges": [
        {
          "range": {
            "start": {
              "line": 7,
              "character": 4
            },
            "end": {
              "line": 7,
              "character": 7
            }
          },
          "text": ""
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 22,
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
    "id": 23,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 3,
        "character": 33
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 24,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 4,
        "character": 24
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 25,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 6,
        "character": 8
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 26,
    "method": "textDocument/hover",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 9,
        "character": 20
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 27,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 6,
          "character": 20
        },
        "end": {
          "line": 6,
          "character": 20
        }
      },
      "context": {
        "diagnostics": []
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 28,
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
    "id": 29,
    "method": "textDocument/hover",
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
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 6,
              "character": 19
            },
            "end": {
              "line": 6,
              "character": 20
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
                    "line": 6,
                    "character": 19
                  },
                  "end": {
                    "line": 6,
                    "character": 20
                  }
                }
              },
              "message": "note: Missing fields: `system`, `id`, `width`, `height`, `title`, `full_screen`, `resizable`"
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 6,
                    "character": 19
                  },
                  "end": {
                    "line": 6,
                    "character": 20
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
        "value": "```nerd\ninit :: fn () -> Self\n```\n\n- Kind: method\n\nCreates an empty frame system ready to allocate frame IDs."
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\ndone :: fn (system: ^Self) -> void\n```\n\n- Kind: method\n\nDestroys any remaining frames and releases all storage owned by the\nsystem. Calling `done` more than once is safe."
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nmain_window\n```\n\n- Kind: local variable\n- Type: `Frame`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\napply :: fn (system: ^Self, frame: ^Frame) -> void\n```\n\n- Kind: method"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 7,
    "result": [
      {
        "title": "Fill missing plex fields",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 6,
                    "character": 20
                  },
                  "end": {
                    "line": 7,
                    "character": 4
                  }
                },
                "newText": "\n        system     : nil\n        id         : 0\n        width      : 0\n        height     : 0\n        title      : \"\"\n        full_screen: no\n        resizable  : no\n    "
              }
            ]
          }
        }
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 8,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nmain_window\n```\n\n- Kind: local variable\n- Type: `Frame`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 9,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nframe_system\n```\n\n- Kind: local variable\n- Type: `FrameSystem`"
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
        "value": "```nerd\nframe_system\n```\n\n- Kind: local variable\n- Type: `FrameSystem`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 13,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\ninit :: fn () -> Self\n```\n\n- Kind: method\n\nCreates an empty frame system ready to allocate frame IDs."
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 14,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\ndone :: fn (system: ^Self) -> void\n```\n\n- Kind: method\n\nDestroys any remaining frames and releases all storage owned by the\nsystem. Calling `done` more than once is safe."
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 15,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nmain_window\n```\n\n- Kind: local variable\n- Type: `Frame`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 16,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\napply :: fn (system: ^Self, frame: ^Frame) -> void\n```\n\n- Kind: method"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 17,
    "result": [
      {
        "title": "Fill missing plex fields",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 6,
                    "character": 20
                  },
                  "end": {
                    "line": 7,
                    "character": 7
                  }
                },
                "newText": "\n        system     : nil\n        id         : 0\n        width      : 0\n        height     : 0\n        title      : \"\"\n        full_screen: no\n        resizable  : no\n    "
              }
            ]
          }
        }
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 18,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nmain_window\n```\n\n- Kind: local variable\n- Type: `Frame`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 19,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nframe_system\n```\n\n- Kind: local variable\n- Type: `FrameSystem`"
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
              "character": 19
            },
            "end": {
              "line": 6,
              "character": 20
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
                    "line": 6,
                    "character": 19
                  },
                  "end": {
                    "line": 6,
                    "character": 20
                  }
                }
              },
              "message": "note: Missing fields: `system`, `id`, `width`, `height`, `title`, `full_screen`, `resizable`"
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 6,
                    "character": 19
                  },
                  "end": {
                    "line": 6,
                    "character": 20
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
    "id": 22,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nframe_system\n```\n\n- Kind: local variable\n- Type: `FrameSystem`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 23,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\ninit :: fn () -> Self\n```\n\n- Kind: method\n\nCreates an empty frame system ready to allocate frame IDs."
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 24,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\ndone :: fn (system: ^Self) -> void\n```\n\n- Kind: method\n\nDestroys any remaining frames and releases all storage owned by the\nsystem. Calling `done` more than once is safe."
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 25,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nmain_window\n```\n\n- Kind: local variable\n- Type: `Frame`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 26,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\napply :: fn (system: ^Self, frame: ^Frame) -> void\n```\n\n- Kind: method"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 27,
    "result": [
      {
        "title": "Fill missing plex fields",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 6,
                    "character": 20
                  },
                  "end": {
                    "line": 7,
                    "character": 4
                  }
                },
                "newText": "\n        system     : nil\n        id         : 0\n        width      : 0\n        height     : 0\n        title      : \"\"\n        full_screen: no\n        resizable  : no\n    "
              }
            ]
          }
        }
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 28,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nmain_window\n```\n\n- Kind: local variable\n- Type: `Frame`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 29,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nframe_system\n```\n\n- Kind: local variable\n- Type: `FrameSystem`"
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
State :: plex { width i32 }
apply :: fn (_state: ^State) {}
main :: fn () {
    state := {}
    apply(^state)
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
        "line": 4,
        "character": 12
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 3,
          "character": 14
        },
        "end": {
          "line": 3,
          "character": 14
        }
      },
      "context": {
        "diagnostics": []
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
              "character": 13
            },
            "end": {
              "line": 3,
              "character": 14
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Plex literal is missing required field",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 3,
                    "character": 13
                  },
                  "end": {
                    "line": 3,
                    "character": 14
                  }
                }
              },
              "message": "note: Missing field: `width`"
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 3,
                    "character": 13
                  },
                  "end": {
                    "line": 3,
                    "character": 14
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
    "id": 2,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nstate\n```\n\n- Kind: local variable\n- Type: `State`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nstate\n```\n\n- Kind: local variable\n- Type: `State`"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": [
      {
        "title": "Fill missing plex fields",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 3,
                    "character": 14
                  },
                  "end": {
                    "line": 3,
                    "character": 14
                  }
                },
                "newText": "\n        width: 0\n    "
              }
            ]
          }
        }
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
        }
    }
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 21,
          "character": 28
        },
        "end": {
          "line": 21,
          "character": 28
        }
      },
      "context": {
        "diagnostics": []
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 22,
          "character": 8
        },
        "end": {
          "line": 22,
          "character": 8
        }
      },
      "context": {
        "diagnostics": []
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 21,
          "character": 9
        },
        "end": {
          "line": 21,
          "character": 9
        }
      },
      "context": {
        "diagnostics": []
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
              "line": 22,
              "character": 8
            },
            "end": {
              "line": 22,
              "character": 8
            }
          },
          "text": "    None => {}\n        "
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 21,
          "character": 28
        },
        "end": {
          "line": 21,
          "character": 28
        }
      },
      "context": {
        "diagnostics": []
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
    "id": 6,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 21,
          "character": 28
        },
        "end": {
          "line": 21,
          "character": 28
        }
      },
      "context": {
        "diagnostics": []
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
    "result": [
      {
        "title": "Add use std.network",
        "kind": "quickfix",
        "edit": {
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
                    "character": 0
                  }
                },
                "newText": "use std.network\n"
              }
            ]
          }
        }
      },
      {
        "title": "Add missing enum variants",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 22,
                    "character": 8
                  },
                  "end": {
                    "line": 22,
                    "character": 8
                  }
                },
                "newText": "\n            None => {\n            }\n\n            Closed => {\n            }\n\n            Resized { width: _, height: _ } => {\n            }\n\n            KeyPress { scan_code: _ } => {\n            }\n\n            KeyRelease { scan_code: _ } => {\n            }\n\n            Character { codepoint: _ } => {\n            }\n        "
              }
            ]
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
        "title": "Add missing enum variants",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 22,
                    "character": 8
                  },
                  "end": {
                    "line": 22,
                    "character": 8
                  }
                },
                "newText": "\n            None => {\n            }\n\n            Closed => {\n            }\n\n            Resized { width: _, height: _ } => {\n            }\n\n            KeyPress { scan_code: _ } => {\n            }\n\n            KeyRelease { scan_code: _ } => {\n            }\n\n            Character { codepoint: _ } => {\n            }\n        "
              }
            ]
          }
        }
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": [
      {
        "title": "Add missing enum variants",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 22,
                    "character": 8
                  },
                  "end": {
                    "line": 22,
                    "character": 8
                  }
                },
                "newText": "\n            None => {\n            }\n\n            Closed => {\n            }\n\n            Resized { width: _, height: _ } => {\n            }\n\n            KeyPress { scan_code: _ } => {\n            }\n\n            KeyRelease { scan_code: _ } => {\n            }\n\n            Character { codepoint: _ } => {\n            }\n        "
              }
            ]
          }
        }
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
    "id": 5,
    "result": [
      {
        "title": "Add use std.network",
        "kind": "quickfix",
        "edit": {
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
                    "character": 0
                  }
                },
                "newText": "use std.network\n"
              }
            ]
          }
        }
      },
      {
        "title": "Add missing enum variants",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 23,
                    "character": 8
                  },
                  "end": {
                    "line": 23,
                    "character": 8
                  }
                },
                "newText": "\n            Closed => {\n            }\n\n            Resized { width: _, height: _ } => {\n            }\n\n            KeyPress { scan_code: _ } => {\n            }\n\n            KeyRelease { scan_code: _ } => {\n            }\n\n            Character { codepoint: _ } => {\n            }\n        "
              }
            ]
          }
        }
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
    "id": 6,
    "result": [
      {
        "title": "Add missing enum variants",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 23,
                    "character": 8
                  },
                  "end": {
                    "line": 23,
                    "character": 8
                  }
                },
                "newText": "\n            Closed => {\n            }\n\n            Resized { width: _, height: _ } => {\n            }\n\n            KeyPress { scan_code: _ } => {\n            }\n\n            KeyRelease { scan_code: _ } => {\n            }\n\n            Character { codepoint: _ } => {\n            }\n        "
              }
            ]
          }
        }
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
use std.vulkan

main :: fn () {
    _info := VkInstanceCreateInfo {
        sType: VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO
        pNext: ^VkDebugUtilsMessengerCreateInfoEXT {
        }
        ...
    }
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 5,
          "character": 52
        },
        "end": {
          "line": 5,
          "character": 52
        }
      },
      "context": {
        "diagnostics": []
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
              "character": 51
            },
            "end": {
              "line": 5,
              "character": 52
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
                    "line": 5,
                    "character": 51
                  },
                  "end": {
                    "line": 5,
                    "character": 52
                  }
                }
              },
              "message": "note: Missing fields: `sType`, `pNext`, `flags`, `messageSeverity`, `messageType`, `pfnUserCallback`, `pUserData`"
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 5,
                    "character": 51
                  },
                  "end": {
                    "line": 5,
                    "character": 52
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
    "id": 2,
    "result": [
      {
        "title": "Fill missing plex fields",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 5,
                    "character": 52
                  },
                  "end": {
                    "line": 6,
                    "character": 8
                  }
                },
                "newText": "\n            sType          : 0\n            pNext          : nil\n            flags          : 0\n            messageSeverity: 0\n            messageType    : 0\n            pfnUserCallback: nil\n            pUserData      : nil\n        "
              }
            ]
          }
        }
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
Callback :: fn (value: i32) -> i32
Callbacks :: plex {
    direct fn (value: i32) -> i32
    aliased Callback
}
main :: fn () {
    _callbacks := Callbacks { }
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 6,
          "character": 29
        },
        "end": {
          "line": 6,
          "character": 29
        }
      },
      "context": {
        "diagnostics": []
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
              "character": 28
            },
            "end": {
              "line": 6,
              "character": 29
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
                    "line": 6,
                    "character": 28
                  },
                  "end": {
                    "line": 6,
                    "character": 29
                  }
                }
              },
              "message": "note: Missing fields: `direct`, `aliased`"
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 6,
                    "character": 28
                  },
                  "end": {
                    "line": 6,
                    "character": 29
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
    "id": 2,
    "result": [
      {
        "title": "Fill missing plex fields",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 6,
                    "character": 29
                  },
                  "end": {
                    "line": 6,
                    "character": 30
                  }
                },
                "newText": "\n        direct : nil\n        aliased: nil\n    "
              }
            ]
          }
        }
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
Callback :: fn (value: i32) -> i32
Callbacks :: plex {
    direct fn (value: i32) -> i32
    aliased Callback
}
main :: fn () {
    _callbacks: Callbacks = { }
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 6,
          "character": 29
        },
        "end": {
          "line": 6,
          "character": 29
        }
      },
      "context": {
        "diagnostics": []
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
              "character": 28
            },
            "end": {
              "line": 6,
              "character": 29
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
                    "line": 6,
                    "character": 28
                  },
                  "end": {
                    "line": 6,
                    "character": 29
                  }
                }
              },
              "message": "note: Missing fields: `direct`, `aliased`"
            },
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 6,
                    "character": 28
                  },
                  "end": {
                    "line": 6,
                    "character": 29
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
    "id": 2,
    "result": [
      {
        "title": "Fill missing plex fields",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 6,
                    "character": 29
                  },
                  "end": {
                    "line": 6,
                    "character": 30
                  }
                },
                "newText": "\n        direct : nil\n        aliased: nil\n    "
              }
            ]
          }
        }
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
Sha1Hash :: plex {
    hash [20]u8
}
impl Display for Sha1Hash {
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 3,
          "character": 0
        },
        "end": {
          "line": 3,
          "character": 0
        }
      },
      "context": {
        "diagnostics": []
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "range": {
        "start": {
          "line": 4,
          "character": 0
        },
        "end": {
          "line": 4,
          "character": 0
        }
      },
      "context": {
        "diagnostics": []
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
          "message": "Trait implementation is missing required member",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/mods/core.n",
                "range": {
                  "start": {
                    "line": 197,
                    "character": 4
                  },
                  "end": {
                    "line": 197,
                    "character": 11
                  }
                }
              },
              "message": "Type `Display` is defined here"
            },
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
              "message": "note: Missing member: `show`"
            },
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
              "message": "help: Add the missing member to this `impl` block."
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
        "title": "Stub missing trait members",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 4,
                    "character": 0
                  },
                  "end": {
                    "line": 4,
                    "character": 0
                  }
                },
                "newText": "    show :: fn (_self: ^Self) -> string {\n        result: string\n        return result\n    }\n"
              }
            ]
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
        "title": "Stub missing trait members",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "file:///test.n": [
              {
                "range": {
                  "start": {
                    "line": 4,
                    "character": 0
                  },
                  "end": {
                    "line": 4,
                    "character": 0
                  }
                },
                "newText": "    show :: fn (_self: ^Self) -> string {\n        result: string\n        return result\n    }\n"
              }
            ]
          }
        }
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
-- lsp-uri: __REPO_URI__/tests/lsp/trait-stub-imports/client.n
use contracts
Hash :: plex { bytes [20]u8 }
impl Printable for Hash {
    reset :: fn (_self: Self) {}
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/trait-stub-imports/client.n"
      },
      "range": {
        "start": {
          "line": 2,
          "character": 0
        },
        "end": {
          "line": 2,
          "character": 0
        }
      },
      "context": {
        "diagnostics": []
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
      "uri": "__REPO_URI__/tests/lsp/trait-stub-imports/client.n",
      "diagnostics": [
        {
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
          "severity": 1,
          "source": "nerd",
          "message": "Trait implementation is missing required member",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/tests/lsp/trait-stub-imports/contracts/mod.n",
                "range": {
                  "start": {
                    "line": 0,
                    "character": 4
                  },
                  "end": {
                    "line": 0,
                    "character": 13
                  }
                }
              },
              "message": "Type `Printable` is defined here"
            },
            {
              "location": {
                "uri": "__REPO_URI__/tests/lsp/trait-stub-imports/client.n",
                "range": {
                  "start": {
                    "line": 2,
                    "character": 0
                  },
                  "end": {
                    "line": 2,
                    "character": 4
                  }
                }
              },
              "message": "note: Missing member: `show`"
            },
            {
              "location": {
                "uri": "__REPO_URI__/tests/lsp/trait-stub-imports/client.n",
                "range": {
                  "start": {
                    "line": 2,
                    "character": 0
                  },
                  "end": {
                    "line": 2,
                    "character": 4
                  }
                }
              },
              "message": "help: Add the missing member to this `impl` block."
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
        "title": "Stub missing trait members",
        "kind": "quickfix",
        "edit": {
          "changes": {
            "__REPO_URI__/tests/lsp/trait-stub-imports/client.n": [
              {
                "range": {
                  "start": {
                    "line": 4,
                    "character": 0
                  },
                  "end": {
                    "line": 4,
                    "character": 0
                  }
                },
                "newText": "\n    show :: fn (_self: Self) -> string {\n        result: string\n        return result\n    }\n"
              }
            ]
          }
        }
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
-- lsp-uri: __REPO_URI__/tests/lsp/trait-stub-imports/client.n
contracts :: use contracts
Hash :: plex { bytes [20]u8 }
impl Printable for Hash {
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/codeAction",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/trait-stub-imports/client.n"
      },
      "range": {
        "start": {
          "line": 2,
          "character": 0
        },
        "end": {
          "line": 2,
          "character": 0
        }
      },
      "context": {
        "diagnostics": []
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
      "uri": "__REPO_URI__/tests/lsp/trait-stub-imports/client.n",
      "diagnostics": [
        {
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
          "severity": 1,
          "source": "nerd",
          "message": "Type mismatch: expected `known trait`, found `Printable`",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/tests/lsp/trait-stub-imports/client.n",
                "range": {
                  "start": {
                    "line": 2,
                    "character": 0
                  },
                  "end": {
                    "line": 2,
                    "character": 4
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
    "result": []
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]
