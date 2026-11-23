test "formats source tests"{value:=6*7
assert value==42,"math still works"}

test{helper::41
make_answer::fn()->i32{return helper+1}}
¬
test "formats source tests" {
    value := 6 * 7
    assert value == 42, "math still works"
}

test {

    helper :: 41

    make_answer :: fn () -> i32 {
        return helper + 1
    }

}
¬
test "maths arbitrary-axis quaternion composition agrees with matrix composition without changing the handedness"{assert yes}
¬
test "maths arbitrary-axis quaternion composition agrees with matrix composition without changing the handedness" {
    assert yes
}

¬
test "fallback source test names must retain one string token even when branch assertions require token formatting"{on yes{yes=>assert yes else=>assert no}}
¬
test "fallback source test names must retain one string token even when branch assertions require token formatting" {
    on yes {
        yes => assert yes else => assert no
    }
}
