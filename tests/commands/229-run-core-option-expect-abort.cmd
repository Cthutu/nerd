use core

main :: fn () {
    maybe : Option[i32] = None
    _ := maybe.expect("missing value")
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
