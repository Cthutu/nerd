main::fn(){
ratio:f64=1.5+2.0*3.0
flags:u32=7|2^1&3
same:=ratio>=7.5&&flags!=0
return on same=>"ok" else "bad"
}
¬
main :: fn () {
    ratio : f64 = 1.5 + 2.0 * 3.0
    flags : u32 = 7 | 2 ^ 1 & 3
    same  :     = ratio >= 7.5 && flags != 0

    return on same => "ok" else "bad"
}
