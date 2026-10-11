use std.io
use test.optional_defaults
local :: fn (value: ?u64 = nil) -> u64 {
    return on value { present => present else => 42 }
}
main :: fn () -> i32 {
    reader : Reader
    assert local() == 42
    assert local(value = 0) == 0
    assert stamp() == 99
    assert stamp(value = nil) == 99
    assert stamp(value = 0) == 0
    assert stamp(value = 7) == 7
    assert decimal() == 1.5
    assert decimal(value = 2.5) == 2.5
    assert reader.read() == 99
    assert reader.read(value = 8) == 8
    prn("Imported optional nil defaults passed")
    return 0
}
¬
0
¬
Imported optional nil defaults passed
¬
hir 0
¬
; nerd llvm-ir 0
