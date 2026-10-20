-- pointers feature scenarios. Results and stdout are checked in order.
use std.io

-- 053-pointers
case_0_main :: fn () -> i32 {
    values: [3]i32 = [10, 20, 30]
    array_ptr: ^[3]i32 = ^values
    elem_ptr: ^i32 = ^values[1]
    literal_ptr: ^[3]i32 = ^[1, 2, 3]

    prn($"array pointer = {array_ptr[0]}")
    prn($"elem pointer = {elem_ptr[0]}")
    prn($"literal pointer = {literal_ptr[0]}")

    return array_ptr[0][0] + elem_ptr[0] + literal_ptr[0][2]
}

-- 078-deref-lvalue
case_1_main :: fn () -> i32 {
    value := 1
    ptr: ^i32 = ^value

    ptr^ = 7

    prn($"{value} {ptr^}")
    return value - 7
}

-- 090-nil-pointers
case_2_take_slice :: fn (bytes: []u8) -> i32 {
    on bytes == nil => return 2
    return 0
}

case_2_ret_ptr :: fn () -> ^i32 {
    return nil
}

case_2_main :: fn () -> i32 {
    value: i32 = 7
    ptr: ^i32 = nil
    bytes: []u8 = nil

    ptr = ^value
    ptr^ = 9
    ptr = nil

    on case_2_take_slice(nil) != 3 - 1 => return 3
    on case_2_take_slice(bytes) != 3 - 1 => return 4
    ptr = case_2_ret_ptr()

    return value - 9
}

-- 095-void-pointer-compat
case_3_accept_void :: fn (ptr: ^void) -> i32 {
    view := ptr.as([]u8, 1)
    return view[0].as(i32) - 7
}

case_3_main :: fn () -> i32 {
    value: u8 = 7
    ptr := ^value
    return case_3_accept_void(ptr)
}

-- 122-pointer-alias-cycle
Case_4_Node :: plex {
    value i32
    next  ^Case_4_Node
}

case_4_nodes: []Case_4_Node = [
    { value: 1, next: case_4_second },
    { value: 2, next: case_4_first },
]

case_4_first  :: ^case_4_nodes[0]
case_4_second :: ^case_4_nodes[1]

case_4_main :: fn () -> i32 {
    return case_4_nodes[0].next.value + case_4_nodes[1].next.value
}

-- 148-untyped-integer-pointer-casts
case_5_main :: fn () -> i32 {
    null_ptr: ^u8 = nil
    display_addr :: 0x1000
    display := display_addr.as(^void)
    value: usize = 0x2000
    other := value.as(^u8)

    on null_ptr != nil => return 1
    on display == nil => return 2
    on other == nil => return 3
    return 0
}

-- 199-pointer-ordering
-- Compatible pointers can be ordered within one allocation.
case_6_main :: fn () -> i32 {
    values: [2]i32 = [10, 20]
    first          := ^values[0]
    second         := ^values[1]

    assert first < second
    assert first <= second
    assert second > first
    assert second >= first
    return 0
}

-- 186-run-address-of-pointer-field
Case_7_Inner :: plex {
    value i32
}

Case_7_Outer :: plex {
    inner Case_7_Inner
}

case_7_write_value :: fn (value: ^i32, replacement: i32) {
    value^ = replacement
}

case_7_main :: fn () -> i32 {
    outer := Case_7_Outer { inner: { value: 1 } }
    outer_ptr := ^outer
    case_7_write_value(^(outer_ptr.inner.value), 42)
    return outer.inner.value - 42
}

-- 188-run-function-pointer-cast
Case_8_Callback :: fn () -> i32

case_8_answer :: fn () -> i32 {
    return 42
}

case_8_main :: fn () -> i32 {
    raw := case_8_answer.as(^void)
    callback := raw.as(Case_8_Callback)
    return callback()
}

main :: fn () -> i32 {
    prn("begin 053-pointers")
    {
        result := case_0_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 33
    }
    prn("begin 078-deref-lvalue")
    {
        result := case_1_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 090-nil-pointers")
    {
        result := case_2_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 095-void-pointer-compat")
    {
        result := case_3_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 122-pointer-alias-cycle")
    {
        result := case_4_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 3
    }
    prn("begin 148-untyped-integer-pointer-casts")
    {
        result := case_5_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 199-pointer-ordering")
    {
        result := case_6_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 186-run-address-of-pointer-field")
    {
        result := case_7_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 188-run-function-pointer-cast")
    {
        result := case_8_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 42
    }
    return 0
}
¬
0
¬
begin 053-pointers
array pointer = [10, 20, 30]
elem pointer = 20
literal pointer = [1, 2, 3]
result 33
begin 078-deref-lvalue
7 7
result 0
begin 090-nil-pointers
result 0
begin 095-void-pointer-compat
result 0
begin 122-pointer-alias-cycle
result 3
begin 148-untyped-integer-pointer-casts
result 0
begin 199-pointer-ordering
result 0
begin 186-run-address-of-pointer-field
result 0
begin 188-run-function-pointer-cast
result 42

¬

¬


=== case: 120-run-pointer-arithmetic ===
Cell :: plex {
    value i32
}

main :: fn () -> i32 {
    values: [4]i32 = [10, 20, 30, 40]
    first := ^values[0]
    third := first + 2
    second := third - 1
    also_third := 2 + first
    distance := third - first

    cells: [2]Cell = [{ value : 7 }, { value : 11 }]
    cell_ptr := ^cells[0]
    next_cell := cell_ptr + 1
    previous_cell := next_cell - 1

    return third[0] + second[0] + also_third[0] + distance.as(i32) +
        next_cell.value + previous_cell.value
}
¬
100
¬

¬

¬

=== case: 121-run-pointer-to-integer-cast ===
main :: fn () -> i32 {
    value : u8 = 7
    ptr := ^value
    addr := ptr.as(usize)

    on addr == 0 => return 1
    return 0
}
¬
0
¬

¬

¬

=== case: 191-run-pointer-to-pointer-cast ===
main :: fn () -> i32 {
    value : u8 = 65
    ptr := (^value).as(^i8)
    return ptr[0].as(i32) - 65
}
¬
0
¬

¬

¬

=== case: 285-run-pointer-ordering ===
main :: fn () -> i32 {
    values: [2]i32 = [10, 20]
    first          := ^values[0]
    second         := ^values[1]

    on !(first < second && first <= second && second > first &&
         second >= first) => return 1
    return 0
}
¬
0
¬

¬

¬

=== case: 323-run-implicit-void-pointer ===
Info :: plex #c { value i32 }
Chain :: plex #c { pNext ^void }
Opaque :: ^void

read :: fn (ptr: ^void) -> i32 { return ptr.as(^Info).value }
erase :: fn (ptr: ^Info) -> ^void { return ptr }
ffi "c" { memcmp(left: ^void, right: ^void, count: usize) -> i32 }

main :: fn () -> i32 {
    info := Info { value: 7 }
    pointer := ^info
    erased: ^void = pointer
    on erased != pointer => return 1
    erased = ^Info { value: 11 }
    on read(erased) != 11 => return 2
    on read(^Info { value: 13 }) != 13 => return 3
    chain := Chain { pNext: ^Info { value: 17 } }
    on read(chain.pNext) != 17 => return 4
    alias: Opaque = ^Info { value: 19 }
    on read(alias) != 19 => return 5
    on erase(pointer) != pointer => return 6
    on read(^info) != 7 => return 7
    on memcmp(^Info { value: 7 }, ^info, Info.size) != 0 => return 8
    prn("implicit void pointers work")
    return 0
}
¬
0
¬
implicit void pointers work

¬

¬

=== case: 171-run-default-nil-pointer-argument ===
use std.io
use std.term
use std.time

simulate :: fn (_sim: TermSimulate) {}

present :: fn (_present: TermPresent) {}

main :: fn () -> i32 {
    term_hook_simulation(from_ms(16), simulate)
    term_hook_presentation(from_ms(100), present)
    prn("default nil pointer ok")
    return 0
}
¬
0
¬
default nil pointer ok

¬

¬
