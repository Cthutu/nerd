use core

main :: fn () {
    maybe : ?i32 = nil
    on maybe => [_present] {
    } else {
        abort("missing value")
    }
}
¬
127
¬
¬
delete
¬
--llvm
¬
run
¬
missing value
