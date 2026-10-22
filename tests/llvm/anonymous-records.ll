Hash :: plex {
    union {
        bytes [20]u8
        plex { a u32 b u32 c u32 d u32 e u32 }
    }
}
main :: fn () -> i32 {
    hash: Hash
    hash.e = 42
    return hash.e.as(i32)
}
¬
  %local.0 = alloca { [5 x i32] }
  %t0 = getelementptr inbounds { [5 x i32] }, ptr %local.0, i64 0, i32 0
  %t1 = getelementptr inbounds { i32, i32, i32, i32, i32 }, ptr %t0, i64 0, i32 4
  store i32 42, ptr %t1
