helper :: fn (_: i32, _: i32, _unused: i32) -> i32 {
    _ := 1
    _ := 2
    _scratch := 10
    return 7
}

value :: fn () -> i32 {
    return 5
}

main :: fn () -> i32 {
    _ := value()
    _ := value()
    _future := 3
    return helper(1, 2, 3)
}
¬
7
¬

¬
delete
¬
--llvm-backend
