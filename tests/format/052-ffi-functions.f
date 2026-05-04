use std.io

libc::"c"
ffi libc abs(i32)->i32
ffi "c"{strlen(^u8)->usize puts(^u8)->i32

setlocale(i32,^u8)->^u8
mmap(addr:^void,len:usize,prot:i32,flags:i32,fd:i32,offset:usize)->^void}
puts :: ffi "c" puts(^u8)
fcntl::ffi"c" fcntl(i32,i32,...)->i32

main::fn(){
value:=abs(-7)
prn($"abs = {value}")
}
¬
use std.io

libc :: "c"

ffi libc abs (i32) -> i32

ffi "c" {
    strlen (^u8) -> usize
    puts   (^u8) -> i32

    setlocale (i32, ^u8) -> ^u8
    mmap      (addr   : ^void,
               len    : usize,
               prot   : i32,
               flags  : i32,
               fd     : i32,
               offset : usize) -> ^void
}

puts :: ffi "c" puts (^u8)

fcntl :: ffi "c" fcntl (i32, i32, ...) -> i32

main :: fn () {
    value := abs(-7)
    prn($"abs = {value}")
}
