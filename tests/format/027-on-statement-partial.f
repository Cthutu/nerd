main :: fn () {
    value :: 2

    on value { 1 => prn("one") }
}
¬
main :: fn () {
    value :: 2

    on value {
        1 => prn("one")
    }
}
