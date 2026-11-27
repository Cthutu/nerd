quote :: '"'
opening :: '{'
closing :: '}'
comment :: '--'
main :: fn () { assert no }
test {
    escaped :: '\''
    slash :: '\\'
}
test "packed quote preserves discovery" {
    assert quote == 34
    assert escaped == 39 && slash == 92
}
test "packed braces preserve bodies" {
    assert opening == 123 && closing == 125
    nested := ['{', '}', '"', '\'']
    assert nested[2] == quote
}
test "packed comment is a value" {
    assert comment == 0x2d2d
}
¬
0
¬
3 tests passed

¬
delete
¬
¬
test
