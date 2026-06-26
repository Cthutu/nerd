use core

choose :: fn (maybe: Option[i32]) -> i32 {
    return on maybe {
        Some(value) => value
        None => abort("missing")
    }
}

main :: fn () -> i32 {
    result := choose(Some(7))
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
