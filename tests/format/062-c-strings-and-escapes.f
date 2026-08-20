ffi "c" puts (text: ^i8) -> i32

main :: fn () {
    puts(c"hello\n")
    text := "a\0b\tc"
}
¬
ffi "c" puts (text: ^i8) -> i32

main :: fn () {
    puts(c"hello\n")
    text := "a\0b\tc"
}
