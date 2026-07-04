use core

main :: fn () -> i32 {
    maybe : Option[i32] = Some(42)
    value := maybe.expect("missing value")
    prn($"{value}")
    return value
}
¬
42
¬
42

¬
delete
¬
--llvm
¬
run
¬
