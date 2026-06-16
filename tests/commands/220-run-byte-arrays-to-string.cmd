--llvm
main :: fn () {
    signed : [5]i8
    signed[0] = 'H'
    signed[1] = 'e'
    signed[2] = 'l'
    signed[3] = 'l'
    signed[4] = 'o'

    unsigned : [5]u8
    unsigned[0] = 'W'
    unsigned[1] = 'o'
    unsigned[2] = 'r'
    unsigned[3] = 'l'
    unsigned[4] = 'd'

    dynamic : [..]i8
    dynamic.push('!')
    dynamic.push('!')

    prn(signed.as(string))
    prn(unsigned.as(string))
    prn(signed[1..4].as(string))
    prn(dynamic.as(string))
    ok : [2]u8 = ['O', 'K']
    prn(ok.as(string))

    dynamic.free()
}
¬
0
¬
Hello
World
ell
!!
OK

¬
delete
