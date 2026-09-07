use equality_values
main :: fn () {
    lhs := [Value {key: 1, cache: 2}]
    rhs := [Value {key: 1, cache: 3}]
    assert lhs[..] == rhs[..]
    a := box[Value]()
    b := box[Value]()
    a.key = 2
    b.key = 2
    a.cache = 5
    b.cache = 6
    assert a == b
    assert a.key == 2 && b.key == 2
}
¬
0
¬

¬
delete
