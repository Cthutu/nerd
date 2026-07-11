use core

choose :: fn (maybe: ?i32) -> i32 {
    return on maybe {
        value => value
        else => abort("missing")
    }
}

main :: fn () -> i32 {
    result := choose(7)
    prn($"{result}")
    return result
}
¬
7
¬
7

¬
delete
¬
--llvm
¬
run
¬
