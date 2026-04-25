libc::"c"
abs::ffi libc(i32)->i32
puts :: ffi "c"(^u8)
fcntl::ffi"c"(i32,i32,...)->i32

main::fn(){
value:=abs(-7)
prn($"abs = {value}")
}
¬
libc :: "c"

abs :: ffi libc (i32) -> i32

puts :: ffi "c" (^u8)

fcntl :: ffi "c" (i32, i32, ...) -> i32

main :: fn () {
    value := abs(-7)
    prn($"abs = {value}")
}
