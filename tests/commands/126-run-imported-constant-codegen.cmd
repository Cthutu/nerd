use test.imported_const_codegen

sink :: fn (a: u32, b: u32, c: u32) -> u32 {
    return a + b + c
}

main :: fn () {
    x := IMPORTED_A.as(u32)
    y := sink(IMPORTED_A, IMPORTED_B | IMPORTED_C, IMPORTED_A)
    prn($"x={x} y={y}")
    on x != 4 => return 1
    on y != 11 => return 2
    return 0
}
¬
0
¬
x=4 y=11
¬
delete
¬
