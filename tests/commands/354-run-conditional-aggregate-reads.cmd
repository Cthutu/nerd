-- Field/index reads materialise aggregate storage even without explicit ^.
-- A skipped first use must not leave later branches reading an empty slot.
Record :: plex { value u64 padding [128]u64 }
Failure :: enum { Invalid }

record_value :: fn (record: Record, first: bool) -> u64\Failure {
    on first && record.value == 99 => return Failure.Invalid!
    return record.value
}

tuple_value :: fn (tuple: ([128]u64, u64), first: bool) -> u64 {
    on first => return tuple.1
    return tuple.1
}

array_value :: fn (array: [128]u64, first: bool) -> u64 {
    on first => return array[0]
    return array[1]
}

local_value :: fn (first: bool) -> u64 {
    record := Record { value: 37 ... }
    on first && record.value == 99 => return 0
    return record.value
}

slice_value :: fn (first: bool) -> u8 {
    bytes: [4]u8 = [5, 6, 7, 8]
    on first => { assert bytes[..][0] == 5 }
    return bytes[..][1]
}

increment :: fn (bytes: []u8) { bytes[0] = bytes[0] + 1 }

loop_slice :: fn () -> u8 {
    bytes: [4]u8
    for _ in [0 .. 2] { increment(bytes[..]) }
    return bytes[0]
}

run :: fn () -> void\Failure {
    record := Record { value: 42 ... }
    assert record_value(record, no)? == 42
    assert record_value(record, yes)? == 42
    padding: [128]u64
    tuple: ([128]u64, u64) = (padding, 11)
    assert tuple_value(tuple, no) == 11
    assert tuple_value(tuple, yes) == 11
    array: [128]u64
    array[0] = 13
    array[1] = 17
    assert array_value(array, no) == 17
    assert array_value(array, yes) == 13
    assert local_value(no) == 37
    assert local_value(yes) == 37
    assert slice_value(no) == 6
    assert slice_value(yes) == 6
    assert loop_slice() == 2
}

main :: fn () -> i32 {
    on run() { _! => return 1 else => return 0 }
}
¬
0
¬

¬
delete
¬
-r
