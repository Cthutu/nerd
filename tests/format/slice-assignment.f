main :: fn(){
xs:=[1,2,3]
xs[1..]=xs[..2]
xs[..2]=7
}
¬
main :: fn () {
    xs := [1, 2, 3]
    xs[1..] = xs[..2]
    xs[..2] = 7
}
¬
-- Slice copies are assignment targets, not a reason to use token fallback.
main::fn(){
a:[8]u8
b:[8]u8
a[a.count-8..]=b[..]
a[..]=b[0..8]
_:=a
}
following::fn()->i32{
value:=-1
return value
}
¬
-- Slice copies are assignment targets, not a reason to use token fallback.
main :: fn () {
    a : [8]u8
    b : [8]u8

    a[a.count - 8..] = b[..]
    a[..] = b[0..8]
    _ := a
}

following :: fn () -> i32 {
    value := -1
    return value
}
