run :: fn () -> void \ Error {
    value := operation()\fallback()\42
    result : i32 \ Error = operation()\42
    broken(&value)
    later : ?i32 \ Error = operation()\42
    grouped := operation()\(42)
    array := operation()\[42]
}
¬
run :: fn () -> void\Error {
    value := operation() \ fallback() \ 42
    result : i32\Error = operation() \ 42
    broken(& value)
    later : ?i32\Error = operation() \ 42
    grouped := operation() \ (42)
    array := operation() \ [42]
}
