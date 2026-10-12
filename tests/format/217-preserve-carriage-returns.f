main :: fn() {
    crlf := "routed\r\n"
    cr := "carriage\rreturn"
    mixed := "first\nsecond\r\n"
    assert crlf.count == 8
    assert cr.count == 15
    assert mixed.count == 14
}
¬
main :: fn () {
    crlf  := "routed\r\n"
    cr    := "carriage\rreturn"
    mixed := "first\nsecond\r\n"

    assert crlf.count == 8
    assert cr.count == 15
    assert mixed.count == 14
}
