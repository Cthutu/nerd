called := no
main :: fn () -> i32\string {
    on !called => {
        called = yes
        value := main()?
        assert value == 7
        return 1
    }
    return 7
}

¬
1

¬

¬
delete

¬

¬
run

¬
