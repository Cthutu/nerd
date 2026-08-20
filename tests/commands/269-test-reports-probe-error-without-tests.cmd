main :: fn () {
    on 1 != 2 {
        return
    }
}
¬
1
¬

¬
delete
¬

¬
test
¬
error: Missing value before Keyword `return`
 --> 269-test-reports-probe-error-without-tests.input.n:3:9
  |
1 | main :: fn () {
2 |     on 1 != 2 {
3 |         return
  |         ^^^^^^ Keyword `return` cannot appear here
4 |     }
5 | }
  |
help: If you intended an if-like conditional, add `=>` before the block: `on
      condition => { ... }`
