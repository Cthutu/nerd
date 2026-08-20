use std.io

libc::"c"
ffi libc abs(value:i32)->i32
ffi "c"{strlen(text:^u8)->usize write_line::puts(text:^u8)->i32
pub seed_rng::srand(seed:u32)

setlocale(category:i32,locale:^u8)->^u8
mmap(addr:^void,len:usize,prot:i32,flags:i32,fd:i32,offset:usize)->^void}
puts :: ffi "c" puts(text:^u8)
fcntl::ffi"c" fcntl(fd:i32,command:i32,...)->i32
syscall::intrinsic"syscall"(number:u64,a0:u64,a1:u64,a2:u64,a3:u64,a4:u64,a5:u64)->i64
pub ffi "c" public_puts(text:^i8)->i32
pub ffi "c"{public_strlen(text:^i8)->usize}

main::fn(){
value:=abs(-7)
prn($"abs = {value}")
}
¬
use std.io

libc :: "c"

ffi libc abs (value: i32) -> i32

ffi "c" {
                    strlen (text: ^u8) -> usize
    write_line   :: puts   (text: ^u8) -> i32
    pub seed_rng :: srand  (seed: u32)

    setlocale (category: i32, locale: ^u8) -> ^u8
    mmap      (addr   : ^void,
               len    : usize,
               prot   : i32,
               flags  : i32,
               fd     : i32,
               offset : usize) -> ^void
}

puts :: ffi "c" puts (text: ^u8)

fcntl :: ffi "c" fcntl (fd: i32, command: i32, ...) -> i32

syscall :: intrinsic "syscall" (number: u64, a0: u64, a1: u64, a2: u64, a3: u64, a4: u64, a5: u64) -> i64

pub ffi "c" public_puts (text: ^i8) -> i32

pub ffi "c" {
    public_strlen (text: ^i8) -> usize
}

main :: fn () {
    value := abs(-7)
    prn($"abs = {value}")
}
