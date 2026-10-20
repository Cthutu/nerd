add :: fn (a: i32, b: i32 = 1, c: i32 = a + b) => a + b + c
main :: fn () => add(20, b = 2)
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 1,
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
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": {
      "signatures": [
        {
          "label": "add(a: i32, b: i32 = 1, c: i32 = a + b) -> i32",
          "documentation": "Named arguments use `name = value`; omitted parameters use declared defaults when available.",
          "parameters": [
            {
              "label": [
                4,
                10
              ]
            },
            {
              "label": [
                12,
                22
              ]
            },
            {
              "label": [
                24,
                38
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 1
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
main :: fn () => add(a = 20, b = 2)
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
        "character": 18
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 1,
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
        "value": "```nerd\nadd :: fn (a: i32, b: i32 = 1, c: i32 = a + b) -> i32\n```\n\n- Kind: function"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "signatures": [
        {
          "label": "add(a: i32, b: i32 = 1, c: i32 = a + b) -> i32",
          "documentation": "Named arguments use `name = value`; omitted parameters use declared defaults when available.",
          "parameters": [
            {
              "label": [
                4,
                10
              ]
            },
            {
              "label": [
                12,
                22
              ]
            },
            {
              "label": [
                24,
                38
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
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
main :: fn () {
    add(
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 2,
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
              "line": 3,
              "character": 0
            },
            "end": {
              "line": 3,
              "character": 1
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Missing value before RightBrace `}`",
          "relatedInformation": [
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
                    "character": 1
                  }
                }
              },
              "message": "help: Insert a literal, parenthesized expression, or unary operator"
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
      "signatures": [
        {
          "label": "add(a: i32, b: i32 = 1, c: i32 = a + b)",
          "documentation": "Named arguments use `name = value`; omitted parameters use declared defaults when available.",
          "parameters": [
            {
              "label": [
                4,
                10
              ]
            },
            {
              "label": [
                12,
                22
              ]
            },
            {
              "label": [
                24,
                38
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
use win

main :: fn () {
    result := DefWindowProcA(
}
¬
[
  {
    "jsonrpc": "2.0",
    "method": "textDocument/didOpen",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/111-imported-ffi-signature-help/main.n",
        "languageId": "nerd",
        "version": 1,
        "text": "use win\n\nmain :: fn () {\n    result := DefWindowProcA(\n}\n"
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "__REPO_URI__/tests/lsp/111-imported-ffi-signature-help/main.n"
      },
      "position": {
        "line": 3,
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
      "uri": "file:///test.n",
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 4,
              "character": 0
            },
            "end": {
              "line": 4,
              "character": 1
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Missing value before RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 4,
                    "character": 0
                  },
                  "end": {
                    "line": 4,
                    "character": 1
                  }
                }
              },
              "message": "help: Insert a literal, parenthesized expression, or unary operator"
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
      "uri": "__REPO_URI__/tests/lsp/111-imported-ffi-signature-help/main.n",
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 4,
              "character": 0
            },
            "end": {
              "line": 4,
              "character": 1
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Missing value before RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "__REPO_URI__/tests/lsp/111-imported-ffi-signature-help/main.n",
                "range": {
                  "start": {
                    "line": 4,
                    "character": 0
                  },
                  "end": {
                    "line": 4,
                    "character": 1
                  }
                }
              },
              "message": "help: Insert a literal, parenthesized expression, or unary operator"
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
      "signatures": [
        {
          "label": "DefWindowProcA(hWnd: HWND, Msg: UINT, wParam: WPARAM, lParam: LPARAM) -> LRESULT",
          "documentation": "Named arguments use `name = value`; omitted parameters use declared defaults when available.",
          "parameters": [
            {
              "label": [
                15,
                25
              ]
            },
            {
              "label": [
                27,
                36
              ]
            },
            {
              "label": [
                38,
                52
              ]
            },
            {
              "label": [
                54,
                68
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
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
main :: fn () {
    add(20,
        2
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/signatureHelp",
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
              "line": 4,
              "character": 0
            },
            "end": {
              "line": 4,
              "character": 1
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected RightParen `)` but found RightBrace `}`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 4,
                    "character": 0
                  },
                  "end": {
                    "line": 4,
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
      "signatures": [
        {
          "label": "add(a: i32, b: i32 = 1, c: i32 = a + b)",
          "documentation": "Named arguments use `name = value`; omitted parameters use declared defaults when available.",
          "parameters": [
            {
              "label": [
                4,
                10
              ]
            },
            {
              "label": [
                12,
                22
              ]
            },
            {
              "label": [
                24,
                38
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 1
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
    missing()
    term_fb_box()
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 4,
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
              "line": 3,
              "character": 4
            },
            "end": {
              "line": 3,
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
                    "line": 3,
                    "character": 4
                  },
                  "end": {
                    "line": 3,
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
      "signatures": [
        {
          "label": "term_fb_box(x: i32, y: i32, w: u32, h: u32, style: BoxStyle = BoxStyle.Single, ink: Colour = COLOUR_TRANSPARENT, paper: Colour = COLOUR_TRANSPARENT) -> void",
          "documentation": "Named arguments use `name = value`; omitted parameters use declared defaults when available.",
          "parameters": [
            {
              "label": [
                12,
                18
              ]
            },
            {
              "label": [
                20,
                26
              ]
            },
            {
              "label": [
                28,
                34
              ]
            },
            {
              "label": [
                36,
                42
              ]
            },
            {
              "label": [
                44,
                77
              ]
            },
            {
              "label": [
                79,
                111
              ]
            },
            {
              "label": [
                113,
                147
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
GLfloat :: f32

ffi "GL" {
    pub glClearColor      (red   : GLfloat,
                           green : GLfloat,
                           blue  : GLfloat,
                           alpha : GLfloat)
}

main :: fn () {
    glClearColor(0.0, 0.0, 0.0, 1.0)
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
        "line": 10,
        "character": 5
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
        "line": 10,
        "character": 5
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 10,
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
      "diagnostics": []
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 2,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nglClearColor :: fn (red: GLfloat, green: GLfloat, blue: GLfloat, alpha: GLfloat) -> void\n```\n\n- Kind: function"
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
          "character": 20
        }
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "signatures": [
        {
          "label": "glClearColor(red: GLfloat, green: GLfloat, blue: GLfloat, alpha: GLfloat) -> void",
          "documentation": "Named arguments use `name = value`; omitted parameters use declared defaults when available.",
          "parameters": [
            {
              "label": [
                13,
                25
              ]
            },
            {
              "label": [
                27,
                41
              ]
            },
            {
              "label": [
                43,
                56
              ]
            },
            {
              "label": [
                58,
                72
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
use std.atomics

main :: fn () {
    value : atomic[i32] = 0
    value.store(1, order = Release)
    scratch := arena(4096, 1024)
    _bytes := scratch.alloc_array[u8](32)
    values : [..]i32
    values.reserve_extra(8)
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
        "character": 10
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 4,
        "character": 34
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
        "character": 30
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
        "character": 16
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 5,
        "character": 31
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
        "line": 6,
        "character": 25
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 8,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 6,
        "character": 40
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
        "line": 8,
        "character": 14
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 10,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 8,
        "character": 26
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
        "value": "```nerd\nstore :: fn (value: T, order :: AtomicStoreOrder = SequentiallyConsistent) -> void\n```\n\n- Kind: atomic method\n- Owner: `atomic[i32]`\n\nStores a value using the selected compile-time memory order."
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "signatures": [
        {
          "label": "store(value: T, order :: AtomicStoreOrder = SequentiallyConsistent) -> void",
          "documentation": "Stores a value using the selected compile-time memory order.",
          "parameters": [
            {
              "label": [
                6,
                14
              ]
            },
            {
              "label": [
                16,
                66
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 1
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "uri": "file:///home/matt/nerd/mods/std/atomics.n",
      "range": {
        "start": {
          "line": 14,
          "character": 4
        },
        "end": {
          "line": 14,
          "character": 11
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
        "value": "```nerd\narena\n```\n\n```nerd\narena :: fn (num_bytes: usize, increment: usize = num_bytes) -> arena\n```\n\n- Kind: built-in type\n- Type: `arena`\n- Notes: opaque, pointer-stable allocation arena\n\nCreates a pointer-stable arena with the requested initial capacity. The optional increment controls how much storage is committed when more space is needed."
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "result": {
      "signatures": [
        {
          "label": "arena(num_bytes: usize, increment: usize = num_bytes) -> arena",
          "documentation": "Creates a pointer-stable arena with the requested initial capacity. The optional increment controls how much storage is committed when more space is needed.",
          "parameters": [
            {
              "label": [
                6,
                22
              ]
            },
            {
              "label": [
                24,
                52
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 1
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 7,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nalloc_array :: fn [T] (count: usize) -> []T\n```\n\n- Kind: arena method\n- Owner: `arena`\n\nAllocates a contiguous slice of count naturally aligned T values from the arena."
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 8,
    "result": {
      "signatures": [
        {
          "label": "alloc_array[T](count: usize) -> []T",
          "documentation": "Allocates a contiguous slice of count naturally aligned T values from the arena.",
          "parameters": [
            {
              "label": [
                15,
                27
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 9,
    "result": {
      "contents": {
        "kind": "markdown",
        "value": "```nerd\nreserve_extra :: fn (additional: usize) -> void\n```\n\n- Kind: dynamic array method\n- Owner: `[..]i32`\n\nEnsures room for the requested number of additional live values."
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 10,
    "result": {
      "signatures": [
        {
          "label": "reserve_extra(additional: usize) -> void",
          "documentation": "Ensures room for the requested number of additional live values.",
          "parameters": [
            {
              "label": [
                14,
                31
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
Settings :: plex { value i32 }
impl Settings {
    configure :: fn (self: ^Self, first: i32, second: i32 = 2) -> i32 {
        return self.value + first + second
    }
    zero :: fn (self: ^Self) -> i32 { return self.value }
    echo :: fn [T] (self: ^Self, argument: T) -> T {
        _ := self.value
        return argument
    }
}
explicit :: fn (self: ^Settings, first: i32) -> i32 { return self.value + first }
configure :: fn (number: i32) -> i32 { return number }
main :: fn () {
    settings : Settings
    _ := settings.configure(1, second = 2)
    _ := explicit(^settings, 1)
    _ := configure(3)
    _ := settings.zero()
    _ := settings.echo(4)
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 15,
        "character": 28
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 15,
        "character": 31
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 16,
        "character": 18
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 17,
        "character": 19
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 18,
        "character": 23
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 7,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 19,
        "character": 23
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
          "text": "Settings :: plex { value i32 }\nimpl Settings {\n    configure :: fn (self: ^Self, first: i32, second: i32 = 2) -> i32 {\n        return self.value + first + second\n    }\n    zero :: fn (self: ^Self) -> i32 { return self.value }\n    echo :: fn [T] (self: ^Self, argument: T) -> T {\n        _ := self.value\n        return argument\n    }\n}\nexplicit :: fn (self: ^Settings, first: i32) -> i32 { return self.value + first }\nconfigure :: fn (number: i32) -> i32 { return number }\nmain :: fn () {\n    settings : Settings\n    _ := settings.configure(1,\n    _ := explicit(^settings, 1)\n    _ := configure(3)\n    _ := settings.zero()\n    _ := settings.echo(4)\n}\n"
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 8,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 15,
        "character": 30
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
      "signatures": [
        {
          "label": "configure(first: i32, second: i32 = 2) -> i32",
          "parameters": [
            {
              "label": [
                10,
                20
              ]
            },
            {
              "label": [
                22,
                37
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "signatures": [
        {
          "label": "configure(first: i32, second: i32 = 2) -> i32",
          "parameters": [
            {
              "label": [
                10,
                20
              ]
            },
            {
              "label": [
                22,
                37
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 1
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "signatures": [
        {
          "label": "explicit(self: ^Settings, first: i32) -> i32",
          "documentation": "Named arguments use `name = value`; omitted parameters use declared defaults when available.",
          "parameters": [
            {
              "label": [
                9,
                24
              ]
            },
            {
              "label": [
                26,
                36
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "result": {
      "signatures": [
        {
          "label": "configure(number: i32) -> i32",
          "documentation": "Named arguments use `name = value`; omitted parameters use declared defaults when available.",
          "parameters": [
            {
              "label": [
                10,
                21
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "result": {
      "signatures": [
        {
          "label": "zero() -> i32",
          "parameters": []
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 7,
    "result": {
      "signatures": [
        {
          "label": "echo[T](argument: T) -> T",
          "parameters": [
            {
              "label": [
                8,
                19
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
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
              "line": 16,
              "character": 6
            },
            "end": {
              "line": 16,
              "character": 7
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Expected RightParen `)` but found Colon `:`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 16,
                    "character": 6
                  },
                  "end": {
                    "line": 16,
                    "character": 7
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
    "id": 8,
    "result": {
      "signatures": [
        {
          "label": "configure(first: i32, second: i32 = 2) -> i32",
          "parameters": [
            {
              "label": [
                10,
                20
              ]
            },
            {
              "label": [
                22,
                37
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 1
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
choose :: fn (debug: bool = no) => debug
main :: fn () {
    enabled := choose(yes)
    _ := enabled
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 2,
        "character": 25
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
          "text": "choose :: fn (debug: bool = no) => debug\nmain :: fn () {\n    enabled := choose(debug = yes)\n    _ := enabled\n}\n"
        }
      ]
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
      "diagnostics": [
        {
          "range": {
            "start": {
              "line": 2,
              "character": 22
            },
            "end": {
              "line": 2,
              "character": 25
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Defaulted parameter `debug` requires a named argument",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 2,
                    "character": 22
                  },
                  "end": {
                    "line": 2,
                    "character": 25
                  }
                }
              },
              "message": "help: Write `debug = ...` or omit the argument to use its default."
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
      "signatures": [
        {
          "label": "choose(debug: bool = no) -> bool",
          "documentation": "Named arguments use `name = value`; omitted parameters use declared defaults when available.",
          "parameters": [
            {
              "label": [
                7,
                23
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
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
        "value": "```nerd\nenabled\n```\n\n- Kind: local variable\n- Type: `bool`"
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
use std.raptor
square :: fn (number: ^i32) -> i32 { return number^ * number^ }
main :: fn () -> ?void {
    scheduler : Scheduler
    scheduler.init(2)?
    queue := scheduler.get_default_queue()
    number : i32 = 4
    task : Task[i32]
    task.create(square, ^number)
    queue.async(^task)?
    _ := task.get()
    assert scheduler.close()
    return
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/signatureHelp",
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
    "id": 3,
    "method": "textDocument/signatureHelp",
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
    "id": 4,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 8,
        "character": 30
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 10,
        "character": 18
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 7,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 9,
        "character": 19
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
          "text": "use std.raptor\nsquare :: fn (number: ^i32) -> i32 { return number^ * number^ }\nmain :: fn () -> ?void {\n    scheduler : Scheduler\n    scheduler.init(2)?\n    queue := scheduler.get_default_queue()\n    number : i32 = 4\n    task : Task[i32]\n    task.create(square,\n    defer assert task.wait()\n    queue.async(^task)?\n    _ := task.get()\n    assert scheduler.close()\n    return\n}\n"
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 8,
        "character": 23
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
      "signatures": [
        {
          "label": "init(workers: usize, capacity: usize = 256) -> ?void",
          "parameters": [
            {
              "label": [
                5,
                19
              ]
            },
            {
              "label": [
                21,
                42
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "signatures": [
        {
          "label": "create[U, T](callback: fn (value: ^U) -> T, argument: ^U)",
          "parameters": [
            {
              "label": [
                13,
                42
              ]
            },
            {
              "label": [
                44,
                56
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "signatures": [
        {
          "label": "create[U, T](callback: fn (value: ^U) -> T, argument: ^U)",
          "parameters": [
            {
              "label": [
                13,
                42
              ]
            },
            {
              "label": [
                44,
                56
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 1
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 5,
    "result": {
      "signatures": [
        {
          "label": "get() -> ?T",
          "parameters": []
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 7,
    "result": {
      "signatures": [
        {
          "label": "async[T](task: ^Task[T]) -> ?void",
          "parameters": [
            {
              "label": [
                9,
                23
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
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
              "character": 4
            },
            "end": {
              "line": 9,
              "character": 9
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Missing value before Keyword `defer`",
          "relatedInformation": [
            {
              "location": {
                "uri": "file:///test.n",
                "range": {
                  "start": {
                    "line": 9,
                    "character": 4
                  },
                  "end": {
                    "line": 9,
                    "character": 9
                  }
                }
              },
              "message": "help: Insert a literal, parenthesized expression, or unary operator"
            }
          ]
        }
      ]
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 6,
    "result": {
      "signatures": [
        {
          "label": "create[U, T](callback: fn (value: ^U) -> T, argument: ^U)",
          "parameters": [
            {
              "label": [
                13,
                42
              ]
            },
            {
              "label": [
                44,
                56
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 1
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
use std.raptor
main :: fn () -> ?void {
    scheduler : Scheduler
    scheduler.init(2)?
    queue := scheduler.get_main_queue()
    _ := scheduler.run_main_tasks()
    assert scheduler.close()
    return
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 4,
        "character": 38
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 5,
        "character": 34
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
              "character": 9
            }
          },
          "severity": 1,
          "source": "nerd",
          "message": "Unused local variable `queue`",
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
                    "line": 4,
                    "character": 4
                  },
                  "end": {
                    "line": 4,
                    "character": 9
                  }
                }
              },
              "message": "help: Remove `queue` or prefix the name with `_` if it is deliberately unused."
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
      "signatures": [
        {
          "label": "get_main_queue() -> ^Queue",
          "parameters": []
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "signatures": [
        {
          "label": "run_main_tasks() -> ?usize",
          "parameters": []
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]

¬
use std.network
main :: fn () -> ?void {
    stream : Socket
    address := loopback(0)
    _ := stream.start_connect(address)
    _ := stream.finish_connect(deadline = nil)
    entries : [1]PollEntry
    _ := poll(entries[..], deadline = nil)
    return
}
¬
[
  {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 4,
        "character": 30
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 5,
        "character": 31
      }
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "method": "textDocument/signatureHelp",
    "params": {
      "textDocument": {
        "uri": "file:///test.n"
      },
      "position": {
        "line": 7,
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
      "signatures": [
        {
          "label": "start_connect(address: Ipv4Address) -> bool\\NetworkError",
          "parameters": [
            {
              "label": [
                14,
                34
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 3,
    "result": {
      "signatures": [
        {
          "label": "finish_connect(deadline: ?Instant = nil) -> bool\\NetworkError",
          "parameters": [
            {
              "label": [
                15,
                39
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 4,
    "result": {
      "signatures": [
        {
          "label": "poll(entries: []PollEntry, deadline: ?Instant = nil) -> usize\\NetworkError",
          "documentation": "Named arguments use `name = value`; omitted parameters use declared defaults when available.",
          "parameters": [
            {
              "label": [
                5,
                25
              ]
            },
            {
              "label": [
                27,
                51
              ]
            }
          ]
        }
      ],
      "activeSignature": 0,
      "activeParameter": 0
    }
  },
  {
    "jsonrpc": "2.0",
    "id": 999,
    "result": null
  }
]
