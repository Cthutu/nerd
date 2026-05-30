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
