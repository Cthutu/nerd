-- test-platform: linux
on "linux" {
    test "active platform test" {
        assert yes
    }
}

on "windows" {
    test "inactive platform test" {
        assert no
    }
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
