test {
    helper_value :: 41

    make_answer :: fn () -> i32 {
        return helper_value + 1
    }
}

test "uses test-only declarations" {
    assert make_answer() == 42
}
¬
0
¬
1 tests passed

¬
delete
¬

¬
test
