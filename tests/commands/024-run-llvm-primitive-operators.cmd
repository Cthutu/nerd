use std.io

half: f32 = 0.5
limit: f64 = 2.0

main :: fn () {
    mask: u32 = 14
    ordered := 3 < 4 && 4 <= 4 && 5 > 4 && 5 >= 5
    prn(on (1.5 < limit && !no) => "float" else "bad")
    prn(on ordered => "cmp" else "bad")
    prn(on (5 % 2 == 1) => "mod" else "bad")
    prn(on (((mask & 11) ^ 3) == 9 || no) => "bits" else "bad")
    return on (half <= 0.5 && 3 != 4) => 1 else 0
}
¬
1
¬
float
cmp
mod
bits

¬
delete
¬
