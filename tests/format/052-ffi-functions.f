use std.print

libc::"c"
ffi libc abs(i32)->i32
puts :: ffi "c" puts(^u8)
fcntl::ffi"c" fcntl(i32,i32,...)->i32

main::fn(){
value:=abs(-7)
prn($"abs = {value}")
}
¬
use std.print
libc :: "c"

ffi libc abs (i32) -> i32
puts :: ffi "c" puts (^u8)

fcntl :: ffi "c" fcntl (i32, i32, ...) -> i32

main :: fn () {
    value := abs(-7)
    prn($"abs = {value}")
}
