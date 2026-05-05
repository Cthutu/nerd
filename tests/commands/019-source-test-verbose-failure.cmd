test "first passes" {
    assert yes
}

test "second fails" {
    assert no, "boom"
}
¬
127
¬
[PASS] source test: first passes

¬
delete
¬
-v
¬
test
¬
assertion failed at 019-source-test-verbose-failure.input.n:7: boom
[FAIL] source test: second fails
source test run failed
