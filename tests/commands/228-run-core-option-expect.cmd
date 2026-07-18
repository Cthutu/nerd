

main :: fn () -> i32 {
    maybe : ?i32 = 42
    value := on maybe => [present] {
        break present
    } else {
        abort("missing value")
    }
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
