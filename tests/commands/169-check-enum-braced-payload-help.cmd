Event :: enum {
    Resized {
        width u16
        height u16
    }
}

main :: fn () {}
¬
1
¬

¬
delete
¬

¬
check
¬
error: Expected LeftParen `(` but found LeftBrace `{`
 --> 169-check-enum-braced-payload-help.input.n:2:13
  |
1 | Event :: enum {
2 |     Resized {
  |             ^ Found LeftBrace `{` here
3 |         width u16
4 |         height u16
  |
note: Braced enum payload shorthand is not supported.
help: Enum variant payloads use parentheses; write `Resized(plex { ... })` for a
      plex payload.
