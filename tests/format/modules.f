pub memory:: use std.memory

main::fn(){
local_memory:: use std.memory
local_memory.kb(1)
memory.mb(1)
}
¬
pub memory :: use std.memory

main :: fn () {
    local_memory :: use std.memory
    local_memory.kb(1)
    memory.mb(1)
}
¬
use   std.io

main::fn(){
p:: use std.io
use p
prn("Hello")
}
¬
use std.io

main :: fn () {
    p :: use std.io
    use p
    prn("Hello")
}
¬
use   std { io }
use test { grouped { alpha beta } }
¬
use std.io
use test.grouped.alpha
use test.grouped.beta
¬
pub use test.folder_pub_use
pub use std { io text }
¬
pub use std.io
pub use std.text
pub use test.folder_pub_use
¬
use zeta.alpha
pub use beta.z
use alpha.beta
pub use beta.a
use std.io

main :: fn () => 0
¬
use alpha.beta
use std.io
use zeta.alpha

pub use beta.a
pub use beta.z

main :: fn () => 0
¬
use zeta
use alpha

main :: fn () => 0
¬
use alpha
use zeta

main :: fn () => 0
¬
pub BLACK:i32=0xff000000
pub RED:i32=0xff0000ff
pub GREEN:i32=0xff00ff00
pub YELLOW:i32=0xff00ffff
¬
pub BLACK  : i32 = 0xff000000
pub RED    : i32 = 0xff0000ff
pub GREEN  : i32 = 0xff00ff00
pub YELLOW : i32 = 0xff00ffff
¬
pub count:i32=0

pub inferred:=1

pub zero:i32

pub uninitialised:i32=undefined

pub typed_constant:i32:2

pub constant::3

private_count:i32=4

private_inferred:=5

pub grouped:i32=6
private_grouped:i32=7

on "windows"{pub guarded:i32=8}

on "linux"{pub inferred_guard:=9}
¬
pub count: i32 = 0

pub inferred := 1

pub zero: i32

pub uninitialised: i32 = undefined

pub typed_constant : i32 : 2

pub constant :: 3

private_count: i32 = 4

private_inferred := 5

pub grouped     : i32 = 6
private_grouped : i32 = 7

on "windows" {

    pub guarded : i32 = 8

}

on "linux" {

    pub inferred_guard := 9

}
