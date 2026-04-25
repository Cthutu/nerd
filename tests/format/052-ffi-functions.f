abs::ffi"c"(i32)->i32
puts :: ffi "c"(^u8)

main::fn(){
value:=abs(-7)
prn($"abs = {value}")
}
¬
abs :: ffi "c" (i32) -> i32

puts :: ffi "c" (^u8)

main :: fn () {
    value := abs(-7)
    prn($"abs = {value}")
}
