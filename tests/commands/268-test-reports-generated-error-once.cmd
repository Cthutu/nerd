test "earlier" {
    assert true
}

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
 --> 268-test-reports-generated-error-once.input.n:7:9
  |
5 | broken :: fn () {
6 |     on 1 != 2 {
7 |         return
  |         ^^^^^^ Keyword `return` cannot appear here
8 |     }
9 | }
  |
help: If you intended an if-like conditional, add `=>` before the block: `on
      condition => { ... }`
source test run failed
