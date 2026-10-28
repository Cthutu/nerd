identity :: fn (text: string) => text
main :: fn () {
    assert $"{identity("a}b")}" == "a}b"
    assert $"{3 > "ab".count.as(i32)}" == "yes"
    assert $"{on yes => "first" else "second"}" == "first"
    assert $"{ $"nested {42}" }" == "nested 42"
    assert $"{["a", "b"][0]}" == "a"
    assert $"{identity("first
second")}" == "first\nsecond"
    assert $"{on 2 {
        1 => "one"
        2 => "two"
        else => "other"
    }}" == "two"
    prn("nested interpolation delimiters passed")
}
¬
0
¬
nested interpolation delimiters passed

¬
hir 0
¬
