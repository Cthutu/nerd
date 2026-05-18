main::fn()->i32{
ptr:^i32=nil
value:i32=7
ptr=^value
ptr^=9
ptr=nil
return value-9
}
¬
main :: fn () -> i32 {
    ptr   : ^i32 = nil
    value : i32  = 7

    ptr = ^value
    ptr^ = 9
    ptr = nil
    return value - 9
}
