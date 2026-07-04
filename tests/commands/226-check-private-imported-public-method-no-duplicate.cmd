use test.private_public_method_import
use test.public_method

main :: fn () -> i32 {
    widget := Widget.init(42)
    return widget.get() - helper()
}
¬
0
¬

¬
delete
¬

¬
check
