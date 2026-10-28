Mob :: plex { name string }
Place :: plex { name string id i32 }
State :: enum { Idle Busy }
Other :: enum { Idle Gone }
main :: fn () {
    name := "gnoll"
    mob := Mob { name }
    _place := Place { name: "forest" id: 1 }
    _another := Mob { name: "bear" }
    prn(mob.name)
    _updated := mob with { name: "orc" }
    state: State = Idle
    on state { Idle => {} Busy => {} }
    _other: Other = Other.Idle
    ptr := ^mob
    prn(ptr.name)
    _qualified: State = State.Idle
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
        "line": 0,
        "character": 14
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
        "line": 0,
        "character": 14
      },
      "newName": "label"
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "method": "textDocument/rename",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 9,
        "character": 12
      },
      "newName": "label"
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "method": "textDocument/rename",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 2,
        "character": 16
      },
      "newName": "Resting"
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "method": "textDocument/rename",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 11,
        "character": 19
      },
      "newName": "Resting"
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 7,
    "method": "textDocument/references",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 9,
        "character": 12
      },
      "context": {
        "includeDeclaration": false
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 8,
    "method": "textDocument/rename",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 1,
        "character": 16
      },
      "newName": "id"
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 9,
    "method": "textDocument/rename",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 2,
        "character": 16
      },
      "newName": "Busy"
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 10,
    "method": "textDocument/rename",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 6,
        "character": 17
      },
      "newName": "label"
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 11,
    "method": "textDocument/rename",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 5,
        "character": 4
      },
      "newName": "creature_name"
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 12,
    "method": "textDocument/rename",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 0,
        "character": 14
      },
      "newName": "fn"
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
          "line": 0,
          "character": 14
        },
        "end": {
          "line": 0,
          "character": 18
        }
      },
      "placeholder": "name"
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
                "line": 0,
                "character": 14
              },
              "end": {
                "line": 0,
                "character": 18
              }
            },
            "newText": "label"
          },
          {
            "range": {
              "start": {
                "line": 6,
                "character": 17
              },
              "end": {
                "line": 6,
                "character": 21
              }
            },
            "newText": "label: name"
          },
          {
            "range": {
              "start": {
                "line": 8,
                "character": 22
              },
              "end": {
                "line": 8,
                "character": 26
              }
            },
            "newText": "label"
          },
          {
            "range": {
              "start": {
                "line": 9,
                "character": 12
              },
              "end": {
                "line": 9,
                "character": 16
              }
            },
            "newText": "label"
          },
          {
            "range": {
              "start": {
                "line": 10,
                "character": 27
              },
              "end": {
                "line": 10,
                "character": 31
              }
            },
            "newText": "label"
          },
          {
            "range": {
              "start": {
                "line": 15,
                "character": 12
              },
              "end": {
                "line": 15,
                "character": 16
              }
            },
            "newText": "label"
          }
        ]
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "changes": {
        "file:///test.n": [
          {
            "range": {
              "start": {
                "line": 0,
                "character": 14
              },
              "end": {
                "line": 0,
                "character": 18
              }
            },
            "newText": "label"
          },
          {
            "range": {
              "start": {
                "line": 6,
                "character": 17
              },
              "end": {
                "line": 6,
                "character": 21
              }
            },
            "newText": "label: name"
          },
          {
            "range": {
              "start": {
                "line": 8,
                "character": 22
              },
              "end": {
                "line": 8,
                "character": 26
              }
            },
            "newText": "label"
          },
          {
            "range": {
              "start": {
                "line": 9,
                "character": 12
              },
              "end": {
                "line": 9,
                "character": 16
              }
            },
            "newText": "label"
          },
          {
            "range": {
              "start": {
                "line": 10,
                "character": 27
              },
              "end": {
                "line": 10,
                "character": 31
              }
            },
            "newText": "label"
          },
          {
            "range": {
              "start": {
                "line": 15,
                "character": 12
              },
              "end": {
                "line": 15,
                "character": 16
              }
            },
            "newText": "label"
          }
        ]
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "result": {
      "changes": {
        "file:///test.n": [
          {
            "range": {
              "start": {
                "line": 2,
                "character": 16
              },
              "end": {
                "line": 2,
                "character": 20
              }
            },
            "newText": "Resting"
          },
          {
            "range": {
              "start": {
                "line": 11,
                "character": 19
              },
              "end": {
                "line": 11,
                "character": 23
              }
            },
            "newText": "Resting"
          },
          {
            "range": {
              "start": {
                "line": 12,
                "character": 15
              },
              "end": {
                "line": 12,
                "character": 19
              }
            },
            "newText": "Resting"
          },
          {
            "range": {
              "start": {
                "line": 16,
                "character": 30
              },
              "end": {
                "line": 16,
                "character": 34
              }
            },
            "newText": "Resting"
          }
        ]
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "result": {
      "changes": {
        "file:///test.n": [
          {
            "range": {
              "start": {
                "line": 2,
                "character": 16
              },
              "end": {
                "line": 2,
                "character": 20
              }
            },
            "newText": "Resting"
          },
          {
            "range": {
              "start": {
                "line": 11,
                "character": 19
              },
              "end": {
                "line": 11,
                "character": 23
              }
            },
            "newText": "Resting"
          },
          {
            "range": {
              "start": {
                "line": 12,
                "character": 15
              },
              "end": {
                "line": 12,
                "character": 19
              }
            },
            "newText": "Resting"
          },
          {
            "range": {
              "start": {
                "line": 16,
                "character": 30
              },
              "end": {
                "line": 16,
                "character": 34
              }
            },
            "newText": "Resting"
          }
        ]
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 7,
    "result": [
      {
        "uri": "file:///test.n",
        "range": {
          "start": {
            "line": 6,
            "character": 17
          },
          "end": {
            "line": 6,
            "character": 21
          }
        }
      },
      {
        "uri": "file:///test.n",
        "range": {
          "start": {
            "line": 8,
            "character": 22
          },
          "end": {
            "line": 8,
            "character": 26
          }
        }
      },
      {
        "uri": "file:///test.n",
        "range": {
          "start": {
            "line": 9,
            "character": 12
          },
          "end": {
            "line": 9,
            "character": 16
          }
        }
      },
      {
        "uri": "file:///test.n",
        "range": {
          "start": {
            "line": 10,
            "character": 27
          },
          "end": {
            "line": 10,
            "character": 31
          }
        }
      },
      {
        "uri": "file:///test.n",
        "range": {
          "start": {
            "line": 15,
            "character": 12
          },
          "end": {
            "line": 15,
            "character": 16
          }
        }
      }
    ]
  },
  {
    "jsonrpc": "2.0",
    "id": 8,
    "result": null
  },
  {
    "jsonrpc": "2.0",
    "id": 9,
    "result": null
  },
  {
    "jsonrpc": "2.0",
    "id": 10,
    "result": {
      "changes": {
        "file:///test.n": [
          {
            "range": {
              "start": {
                "line": 0,
                "character": 14
              },
              "end": {
                "line": 0,
                "character": 18
              }
            },
            "newText": "label"
          },
          {
            "range": {
              "start": {
                "line": 6,
                "character": 17
              },
              "end": {
                "line": 6,
                "character": 21
              }
            },
            "newText": "label: name"
          },
          {
            "range": {
              "start": {
                "line": 8,
                "character": 22
              },
              "end": {
                "line": 8,
                "character": 26
              }
            },
            "newText": "label"
          },
          {
            "range": {
              "start": {
                "line": 9,
                "character": 12
              },
              "end": {
                "line": 9,
                "character": 16
              }
            },
            "newText": "label"
          },
          {
            "range": {
              "start": {
                "line": 10,
                "character": 27
              },
              "end": {
                "line": 10,
                "character": 31
              }
            },
            "newText": "label"
          },
          {
            "range": {
              "start": {
                "line": 15,
                "character": 12
              },
              "end": {
                "line": 15,
                "character": 16
              }
            },
            "newText": "label"
          }
        ]
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 11,
    "result": {
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
                "character": 8
              }
            },
            "newText": "creature_name"
          },
          {
            "range": {
              "start": {
                "line": 6,
                "character": 17
              },
              "end": {
                "line": 6,
                "character": 21
              }
            },
            "newText": "name: creature_name"
          }
        ]
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 12,
    "result": null
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]
¬
use editor_members
main :: fn () {
    mob := make_mob()
    prn(mob.name)
    _value := Mob { name: "bear" }
    state: State = Idle
    on state { Idle => {} Busy => {} }
    _other: State = State.Idle
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
        "line": 3,
        "character": 12
      },
      "newName": "label"
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
        "line": 7,
        "character": 26
      },
      "newName": "Resting"
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
      "changes": {
        "file:///test.n": [
          {
            "range": {
              "start": {
                "line": 3,
                "character": 12
              },
              "end": {
                "line": 3,
                "character": 16
              }
            },
            "newText": "label"
          },
          {
            "range": {
              "start": {
                "line": 4,
                "character": 20
              },
              "end": {
                "line": 4,
                "character": 24
              }
            },
            "newText": "label"
          }
        ],
        "__REPO_URI__/tests/mods/editor_members.n": [
          {
            "range": {
              "start": {
                "line": 0,
                "character": 18
              },
              "end": {
                "line": 0,
                "character": 22
              }
            },
            "newText": "label"
          },
          {
            "range": {
              "start": {
                "line": 2,
                "character": 40
              },
              "end": {
                "line": 2,
                "character": 44
              }
            },
            "newText": "label"
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
                "line": 5,
                "character": 19
              },
              "end": {
                "line": 5,
                "character": 23
              }
            },
            "newText": "Resting"
          },
          {
            "range": {
              "start": {
                "line": 6,
                "character": 15
              },
              "end": {
                "line": 6,
                "character": 19
              }
            },
            "newText": "Resting"
          },
          {
            "range": {
              "start": {
                "line": 7,
                "character": 26
              },
              "end": {
                "line": 7,
                "character": 30
              }
            },
            "newText": "Resting"
          }
        ],
        "__REPO_URI__/tests/mods/editor_members.n": [
          {
            "range": {
              "start": {
                "line": 1,
                "character": 20
              },
              "end": {
                "line": 1,
                "character": 24
              }
            },
            "newText": "Resting"
          },
          {
            "range": {
              "start": {
                "line": 3,
                "character": 45
              },
              "end": {
                "line": 3,
                "character": 49
              }
            },
            "newText": "Resting"
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
Cell :: plex [T] { value T }
main :: fn () {
    first := Cell[i32] { value: 1 }
    second := Cell[string] { value: "two" }
    prn($"{first.value} {second.value}")
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
        "character": 19
      },
      "newName": "item"
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
        "character": 25
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
    "id": 2,
    "result": {
      "changes": {
        "file:///test.n": [
          {
            "range": {
              "start": {
                "line": 0,
                "character": 19
              },
              "end": {
                "line": 0,
                "character": 24
              }
            },
            "newText": "item"
          },
          {
            "range": {
              "start": {
                "line": 2,
                "character": 25
              },
              "end": {
                "line": 2,
                "character": 30
              }
            },
            "newText": "item"
          },
          {
            "range": {
              "start": {
                "line": 3,
                "character": 29
              },
              "end": {
                "line": 3,
                "character": 34
              }
            },
            "newText": "item"
          },
          {
            "range": {
              "start": {
                "line": 4,
                "character": 17
              },
              "end": {
                "line": 4,
                "character": 22
              }
            },
            "newText": "item"
          },
          {
            "range": {
              "start": {
                "line": 4,
                "character": 32
              },
              "end": {
                "line": 4,
                "character": 37
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
    "id": 3,
    "result": {
      "changes": {
        "file:///test.n": [
          {
            "range": {
              "start": {
                "line": 0,
                "character": 19
              },
              "end": {
                "line": 0,
                "character": 24
              }
            },
            "newText": "item"
          },
          {
            "range": {
              "start": {
                "line": 2,
                "character": 25
              },
              "end": {
                "line": 2,
                "character": 30
              }
            },
            "newText": "item"
          },
          {
            "range": {
              "start": {
                "line": 3,
                "character": 29
              },
              "end": {
                "line": 3,
                "character": 34
              }
            },
            "newText": "item"
          },
          {
            "range": {
              "start": {
                "line": 4,
                "character": 17
              },
              "end": {
                "line": 4,
                "character": 22
              }
            },
            "newText": "item"
          },
          {
            "range": {
              "start": {
                "line": 4,
                "character": 32
              },
              "end": {
                "line": 4,
                "character": 37
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
First :: plex { value i32 }
Second :: plex { value i32 }
main :: fn () {
    first := First { value: 1 }
    second := Second { value: 2 }
    prn($"{first.value} {second.value}")
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
        "character": 16
      },
      "newName": "first_value"
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
        "character": 17
      },
      "newName": "second_value"
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
      "changes": {
        "file:///test.n": [
          {
            "range": {
              "start": {
                "line": 0,
                "character": 16
              },
              "end": {
                "line": 0,
                "character": 21
              }
            },
            "newText": "first_value"
          },
          {
            "range": {
              "start": {
                "line": 3,
                "character": 21
              },
              "end": {
                "line": 3,
                "character": 26
              }
            },
            "newText": "first_value"
          },
          {
            "range": {
              "start": {
                "line": 5,
                "character": 17
              },
              "end": {
                "line": 5,
                "character": 22
              }
            },
            "newText": "first_value"
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
                "character": 17
              },
              "end": {
                "line": 1,
                "character": 22
              }
            },
            "newText": "second_value"
          },
          {
            "range": {
              "start": {
                "line": 4,
                "character": 23
              },
              "end": {
                "line": 4,
                "character": 28
              }
            },
            "newText": "second_value"
          },
          {
            "range": {
              "start": {
                "line": 5,
                "character": 32
              },
              "end": {
                "line": 5,
                "character": 37
              }
            },
            "newText": "second_value"
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
First :: enum { Idle Busy }
Second :: enum { Idle Busy }
main :: fn () {
    first: First = Idle
    second: Second = Idle
    _qualified: Second = Second.Idle
    on first { Idle => {} Busy => {} }
    on second { Idle => {} Busy => {} }
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
        "character": 16
      },
      "newName": "Resting"
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
        "character": 17
      },
      "newName": "Waiting"
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
      "changes": {
        "file:///test.n": [
          {
            "range": {
              "start": {
                "line": 0,
                "character": 16
              },
              "end": {
                "line": 0,
                "character": 20
              }
            },
            "newText": "Resting"
          },
          {
            "range": {
              "start": {
                "line": 3,
                "character": 19
              },
              "end": {
                "line": 3,
                "character": 23
              }
            },
            "newText": "Resting"
          },
          {
            "range": {
              "start": {
                "line": 6,
                "character": 15
              },
              "end": {
                "line": 6,
                "character": 19
              }
            },
            "newText": "Resting"
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
                "character": 17
              },
              "end": {
                "line": 1,
                "character": 21
              }
            },
            "newText": "Waiting"
          },
          {
            "range": {
              "start": {
                "line": 4,
                "character": 21
              },
              "end": {
                "line": 4,
                "character": 25
              }
            },
            "newText": "Waiting"
          },
          {
            "range": {
              "start": {
                "line": 5,
                "character": 32
              },
              "end": {
                "line": 5,
                "character": 36
              }
            },
            "newText": "Waiting"
          },
          {
            "range": {
              "start": {
                "line": 7,
                "character": 16
              },
              "end": {
                "line": 7,
                "character": 20
              }
            },
            "newText": "Waiting"
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
Mob :: plex { name string }
main :: fn () {
    mob := Mob { name: "gnoll" }
    on mob {
        Mob { name } => { prn(name) }
    }
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
        "character": 14
      },
      "newName": "label"
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
      "changes": {
        "file:///test.n": [
          {
            "range": {
              "start": {
                "line": 0,
                "character": 14
              },
              "end": {
                "line": 0,
                "character": 18
              }
            },
            "newText": "label"
          },
          {
            "range": {
              "start": {
                "line": 2,
                "character": 17
              },
              "end": {
                "line": 2,
                "character": 21
              }
            },
            "newText": "label"
          },
          {
            "range": {
              "start": {
                "line": 4,
                "character": 14
              },
              "end": {
                "line": 4,
                "character": 18
              }
            },
            "newText": "label: name"
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
