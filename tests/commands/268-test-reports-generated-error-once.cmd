broken :: fn () {
    on 1 != 2 {
        return
    }
}

test "broken" {}
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
 --> 268-test-reports-generated-error-once.input.n:3:9
  |
1 | broken :: fn () {
2 |     on 1 != 2 {
3 |         return
  |         ^^^^^^ Keyword `return` cannot appear here
4 |     }
5 | }
  |
help: If you intended an if-like conditional, add `=>` before the block: `on
      condition => { ... }`
source test run failed
