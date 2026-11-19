=== case: dynamic-array-pointer-members ===
copy :: fn (destination: ^[..]u8, text: string) {
    destination.resize_undefined_to(text.count)
    destination^[..] = text.as([]u8)
    assert destination.count == text.count
}
nested :: fn (destination: ^^[..]u8) {
    destination.push('!')
    assert destination.count == 6
}
main :: fn () {
    bytes: [..]u8
    defer bytes.free()
    copy(^bytes, "hello")
    pointer := ^bytes
    nested(^pointer)
    assert bytes[..].as(string) == "hello!"
    pointer.clear()
    assert bytes.count == 0
}
¬
0
¬

¬

¬
