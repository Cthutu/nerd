use std.print

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
    write_line(c"ffi ok")
}
¬
0
¬
ffi ok

¬
¬
