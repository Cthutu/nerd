main :: fn () {
    values := ((1, 2), (3, (4, 5)), 6)
    assert (values.0).0 == 1
    assert (values.0).1 == 2
    assert (values.1).0 == 3
    assert ((values.1).1).0 == 4
    assert ((values.1).1).1 == 5
    assert values.2 == 6
}
¬
0
¬

¬
delete
