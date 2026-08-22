Failure :: enum {
    Bad
}

implicit_success :: fn () -> void\Failure {
}

explicit_success :: fn () -> void\Failure {
    return
}

both_succeed :: fn () -> void\Failure {
    implicit_success()?
    explicit_success()?
}

main :: fn () -> i32 {
    on both_succeed() => {
        return 0
    } else {
        return 1
    }
}
¬
0
¬

¬
delete
