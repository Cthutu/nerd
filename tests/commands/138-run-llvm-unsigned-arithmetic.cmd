main :: fn () -> i32 {
    high := 0xffff_ffff_ffff_fffe.as(u64)

    on high % 4 != 2 => return 1
    on high / 2 != high >> 1 => return 2
    on high >> 1 <= 0x7fff_ffff.as(u64) => return 3

    return 0
}
¬
0
¬

¬
delete
¬
--llvm
