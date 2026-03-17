- Handle didOpen and didChange notifications from the language server.  This is
  required to get the language server to send us diagnostics, which is the only
  way to test that the compiler is working correctly.  I will add more tests
  here as I build up the compiler.

- Add support for hovering over integers in the LSP

- Add arithmetic binary and unary operations to the compiler so we can do more
  complex expressions: `+ - * / % << >> & | ^ ~ ( )`
