use std.io

libc :: "c"

seed_rng :: ffi libc srand (u32)
write_line :: ffi ("c") puts (^u8) -> i32

Point :: plex #c {
    x i32
    y i32
}

Packed :: plex #packed {
    tag   u8
    value i32
}

Blob :: union {
    i i32
    f f32
}

accept_point_ffi :: ffi libc accept_point (Point)
accept_packed_ffi :: ffi libc accept_packed (Packed)
flip_blob_ffi :: ffi libc flip_blob (Blob) -> Blob

main :: fn () {
    seed_rng(1)
    _ := write_line(c"ffi ok")
}
¬
0
¬
ffi ok

¬
hir 0
module module.0(088-ffi-consolidation.input)
import module.1(std.io)
import import.0 pr from module.1(std.io).decl.9: fn (string) -> void
import import.1 epr from module.1(std.io).decl.10: fn (string) -> void
import import.2 prn from module.1(std.io).decl.11: fn (string) -> void
import import.3 eprn from module.1(std.io).decl.12: fn (string) -> void
import import.4 input from module.1(std.io).decl.13: fn (string) -> string
bind pr = import.0
bind epr = import.1
bind prn = import.2
bind eprn = import.3
bind input = import.4
bind libc = value.0
bind seed_rng = fn.0
bind write_line = fn.1
bind Point = type.0
bind Packed = type.1
bind Blob = type.2
bind accept_point_ffi = fn.2
bind accept_packed_ffi = fn.3
bind flip_blob_ffi = fn.4
bind main = fn.5
type type.0 = Point
type type.1 = Packed
type type.2 = Blob
const value.0: string = string "c"
extern func fn.0(u32) -> void
extern func fn.1(^u8) -> i32
extern func fn.2(Point) -> void
extern func fn.3(Packed) -> void
extern func fn.4(Blob) -> Blob
func fn.5() -> void {
  expr void call bind.6(seed_rng)(u32 1)
  let _: i32 = i32 call bind.7(write_line)(^u8 c"ffi ok")
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [2 x i8] c"c\00"
@.str.m0.1 = private unnamed_addr constant [2 x i8] c"c\00"
@.str.m0.2 = private unnamed_addr constant [7 x i8] c"ffi ok\00"

declare void @$pr({ ptr, i64 })
declare void @$epr({ ptr, i64 })
declare void @$prn({ ptr, i64 })
declare void @$eprn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

declare void @seed_rng(i32)

declare i32 @write_line(ptr)

declare void @accept_point_ffi({ i32, i32 })

declare void @accept_packed_ffi({ i8, i32 })

declare i32 @flip_blob_ffi(i32)

define void @fn.5() {
  call void @seed_rng(i32 1)
  %t0 = call i32 @write_line(ptr @.str.m0.2)
  ret void
}

@$main = alias void (), ptr @fn.5
