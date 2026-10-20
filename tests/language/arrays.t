-- arrays feature scenarios. Results and stdout are checked in order.
use std.io

-- 052-fixed-arrays
case_0_main :: fn () -> i32 {
    xs :: [1, 2, 3]
    ys: [2]i32 = [4, 5]

    prn($"xs = {xs[0]}, {xs[2]}")
    prn($"xs array = {xs}")
    prn($"ys = {ys[1]}")
    prn($"ys array = {ys}")

    return xs[1] + ys[0] + ys[1]
}

-- 054-slices
case_1_main :: fn () -> i32 {
    values: [5]i32 = [10, 20, 30, 40, 50]
    all: []i32 = values[..]
    middle: []i32 = values[1..4]
    from_start: []i32 = values[..3]
    to_end: []i32 = values[2..]
    literal: []i32 = [1, 2, 3][..]
    reslice: []i32 = all[1..3]

    prn($"all = {all}")
    prn($"middle = {middle}")
    prn($"from_start = {from_start}")
    prn($"to_end = {to_end}")
    prn($"literal = {literal}")
    prn($"reslice count = {reslice.count}")
    prn($"data first = {middle.data[0]}")

    return middle[1] + from_start[2] + to_end[0] + literal[2]
}

-- 055-string-slices
case_2_main :: fn () -> i32 {
    text :: "hello"
    whole: string = text[..]
    ell: string = text[1..4]
    tail: string = text[2..]
    head: string = text[..2]

    prn($"whole = {whole}")
    prn($"ell = {ell}")
    prn($"tail = {tail}")
    prn($"head = {head}")
    prn($"count = {ell.count}")
    prn($"first byte = {ell.data[0]}")

    result :: on ell {
        "ell" => 7
        else => 1
    }

    return result + ell.data[1].as(i32)
}

-- 080-dynamic-slice-bounds
case_3_main :: fn () -> i32 {
    values: [5]i32 = [10, 20, 30, 40, 50]
    all: []i32 = values[..]
    end :: all.count - 1

    prefix: []i32 = values[.. end]
    middle: []i32 = values[1 .. end]
    nested: []i32 = values[.. all.count - 2]

    prn($"{prefix}")
    prn($"{middle}")
    prn($"{nested}")

    return prefix[3] + middle[2] + nested[2]
}

-- 081-nested-array-literals
Case_4_RoomType :: enum { NONE HALL KITCHEN }

Case_4_Room :: plex {
    exits [2]Case_4_RoomType
}

case_4_rooms : [2]Case_4_Room : [
    { exits: [NONE, HALL] },
    { exits: [KITCHEN, NONE] },
]

case_4_main :: fn () -> i32 {
    prn($"{case_4_rooms[0].exits[0] == NONE}")
    prn($"{case_4_rooms[0].exits[1] == HALL}")
    prn($"{case_4_rooms[1].exits[0] == KITCHEN}")
    prn($"{case_4_rooms[1].exits[1] == NONE}")
    return 0
}

-- 093-slice-casts-and-nil
case_5_main :: fn () -> i32 {
    text: [4]u8 = ['a', 'b', 'c', 'd']
    ptr := text[..].data
    view := ptr.as([]u8, 2)
    empty: []u8 = nil

    return view.count.as(i32) * 100 + view[0].as(i32) + empty.count.as(i32)
}

-- 096-nil-slice-equality
case_6_main :: fn () -> i32 {
    empty: []u8 = nil
    text: [2]u8 = ['a', 'b']
    view := text[..]

    on empty != nil => return 1
    on nil != empty => return 2
    on view == nil => return 3
    on nil == view => return 4

    return 0
}

-- 097-return-nil-slice
case_7_make :: fn (ok: bool) -> []u8 {
    on ok == no => return nil

    text: [2]u8 = ['o', 'k']
    return text[..]
}

case_7_main :: fn () -> i32 {
    empty := case_7_make(no)
    full := case_7_make(yes)

    on empty != nil => return 1
    on full == nil => return 2
    return 0
}

-- 099-dynamic-arrays
case_8_make_words :: fn () -> [..]string {
    words: [..]string
    words.push("look")
    words.push("north")
    return words
}

case_8_main :: fn () -> i32 {
    empty: [..]string
    on empty != nil => return 1
    on empty.count != 0 => return 2
    on empty.capacity != 0 => return 3

    names: [4..]string = ["north", "south"]
    on names.count != 2 => return 4
    on names.capacity < 4 => return 5

    names.push("east")
    on names.count != 3 => return 6

    extra :: ["west", "up"]
    names.append(extra[..])
    on names.count != 5 => return 7

    last := names.pop()
    on last != "up" => return 8
    on names.count != 4 => return 9
    names.push(last)

    view := names[..]
    on view.count != 5 => return 10

    prn($"{view[0]} {view[1]} {view[2]} {view[3]} {view[4]}")

    names.reserve_to(10)
    on names.capacity < 10 => return 11

    words := case_8_make_words()
    on words.count != 2 => return 12
    prn($"{words[0]} {words[1]}")
    words.free()

    nums: [..]i32
    nums.push(1)
    nums.push(2)
    nums.push(3)
    nums.push(4)
    nums.push(5)
    nums.delete(1)
    on nums.count != 4 => return 13
    on nums[0] != 1 => return 14
    on nums[1] != 3 => return 15
    on nums[2] != 4 => return 16
    on nums[3] != 5 => return 17
    nums.swap_delete(1)
    on nums.count != 3 => return 18
    on nums[0] != 1 => return 19
    on nums[1] != 5 => return 20
    on nums[2] != 4 => return 21
    nums.free()

    names.clear()
    on names.count != 0 => return 22
    names.free()
    on names != nil => return 23
    return 0
}

-- 101-dynarray-typed-locals
case_9_split :: fn (s: string, sep: string) -> [..]string {
    parts: [..]string

    on sep.count == 0 => {
        parts.push(s)
        return parts
    }

    start: usize = 0
    i: usize = 0
    for i + sep.count <= s.count {
        matched := yes
        for j: usize = 0; matched && j < sep.count; j += 1 {
            on s.data[i + j] != sep.data[j] => matched = no
        }

        on matched => {
            parts.push(s[start .. i])
            i += sep.count
            start = i
        } else {
            i += 1
        }
    }

    parts.push(s[start ..])
    return parts
}

case_9_main :: fn () -> i32 {
    parts := case_9_split("look around", " ")
    on parts.count != 2 => return 1
    on parts[0] != "look" => return 2
    on parts[1] != "around" => return 3
    return 0
}

-- 119-slice-element-pointers
case_10_main :: fn () -> i32 {
    fixed: [3]i32 = [10, 20, 30]
    fixed_ptr: ^i32 = ^fixed[1]
    fixed_ptr^ = 21

    slice: []i32 = fixed[..]
    slice_ptr: ^i32 = ^slice[2]
    slice_ptr^ = 32

    dyn: [..]i32
    dyn.push(5)
    dyn.push(6)
    dyn_ptr: ^i32 = ^dyn[1]
    dyn_ptr^ = 7

    text := "abc"
    char_ptr: ^u8 = ^text[1]

    prn($"{fixed[1]} {slice[2]} {dyn[1]} {char_ptr^}")

    result := fixed[1] + slice[2] + dyn[1] + char_ptr^.as(i32)
    dyn.free()
    return result
}

-- 121-self-referential-slice-literal
Case_11_Node :: plex {
    value i32
    next  ^Case_11_Node
}

case_11_nodes: []Case_11_Node = [
    { value: 1, next: ^case_11_nodes[1] },
    { value: 2, ... },
]

case_11_main :: fn () -> i32 {
    return case_11_nodes[0].next.value
}

-- 124-fixed-array-pointer-cycles
Case_12_Node :: plex {
    value i32
    next  ^Case_12_Node
}

case_12_direct: [2]Case_12_Node = [
    { value: 1, next: ^case_12_direct[1] },
    { value: 2, ... },
]

case_12_aliased: [2]Case_12_Node = [
    { value: 3, next: case_12_second },
    { value: 4, next: case_12_first },
]

case_12_first  :: ^case_12_aliased[0]
case_12_second :: ^case_12_aliased[1]

case_12_main :: fn () -> i32 {
    return case_12_direct[0].next.value + case_12_aliased[0].next.value + case_12_aliased[1].next.value
}

-- 143-dynarray-runtime-capacity-and-resize
case_13_make :: fn (initial_size: usize) -> i32 {
    values: [initial_size..]i32
    on values.capacity < initial_size => return 1
    on values.count != 0 => return 2

    values.resize_undefined_to(initial_size)
    values[0] = 7
    values[1] = 8
    values[2] = 9

    values.resize_to(5)
    on values.count != 5 => return 3
    on values[3] != 0 => return 4
    on values[4] != 0 => return 5

    result := values[0] + values[1] + values[2]
    values.free()
    return result
}

case_13_main :: fn () -> i32 {
    return case_13_make(3)
}

-- 235-run-pointer-dynarray-count-logical
case_14_check :: fn (output: ^[..]u8, max: usize) -> bool {
    return max != 0 && output.count >= max
}

case_14_main :: fn () -> i32 {
    bytes : [..]u8
    defer bytes.free()
    bytes.push(1)

    on !case_14_check(^bytes, 2) => return 0
    return 1
}

-- 265-run-byte-slice-equality
case_15_main :: fn () -> i32 {
    data: []u8 = [3, 4, 5, 6, 7, 6, 7]

    assert data == [3, 4, 5, 6, 7, 6, 7]
    assert data != [3, 4, 5, 6, 7, 6, 8]
    assert data != [3, 4, 5]
    return 0
}

-- 266-run-mutable-slice-literal
case_16_main :: fn () -> i32 {
    data: []u8 = [3, 4, 5]
    data[1] = 9

    assert data == [3, 9, 5]
    return 0
}

-- 267-run-fixed-array-equality
case_17_main :: fn () -> i32 {
    data: [5]u8 = [42, 42, 42, 42, 42]

    assert data == [42, 42, 42, 42, 42]
    assert data != [42, 42, 42, 42, 41]
    return 0
}

main :: fn () -> i32 {
    prn("begin 052-fixed-arrays")
    {
        result := case_0_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 11
    }
    prn("begin 054-slices")
    {
        result := case_1_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 93
    }
    prn("begin 055-string-slices")
    {
        result := case_2_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 115
    }
    prn("begin 080-dynamic-slice-bounds")
    {
        result := case_3_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 110
    }
    prn("begin 081-nested-array-literals")
    {
        result := case_4_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 093-slice-casts-and-nil")
    {
        result := case_5_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 41
    }
    prn("begin 096-nil-slice-equality")
    {
        result := case_6_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 097-return-nil-slice")
    {
        result := case_7_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 099-dynamic-arrays")
    {
        result := case_8_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 101-dynarray-typed-locals")
    {
        result := case_9_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 119-slice-element-pointers")
    {
        result := case_10_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 158
    }
    prn("begin 121-self-referential-slice-literal")
    {
        result := case_11_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 2
    }
    prn("begin 124-fixed-array-pointer-cycles")
    {
        result := case_12_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 9
    }
    prn("begin 143-dynarray-runtime-capacity-and-resize")
    {
        result := case_13_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 24
    }
    prn("begin 235-run-pointer-dynarray-count-logical")
    {
        result := case_14_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 265-run-byte-slice-equality")
    {
        result := case_15_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 266-run-mutable-slice-literal")
    {
        result := case_16_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 267-run-fixed-array-equality")
    {
        result := case_17_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    return 0
}
¬
0
¬
begin 052-fixed-arrays
xs = 1, 3
xs array = [1, 2, 3]
ys = 5
ys array = [4, 5]
result 11
begin 054-slices
all = [10, 20, 30, 40, 50]
middle = [20, 30, 40]
from_start = [10, 20, 30]
to_end = [30, 40, 50]
literal = [1, 2, 3]
reslice count = 2
data first = 20
result 93
begin 055-string-slices
whole = hello
ell = ell
tail = llo
head = he
count = 3
first byte = 101
result 115
begin 080-dynamic-slice-bounds
[10, 20, 30, 40]
[20, 30, 40]
[10, 20, 30]
result 110
begin 081-nested-array-literals
yes
yes
yes
yes
result 0
begin 093-slice-casts-and-nil
result 41
begin 096-nil-slice-equality
result 0
begin 097-return-nil-slice
result 0
begin 099-dynamic-arrays
north south east west up
look north
result 0
begin 101-dynarray-typed-locals
result 0
begin 119-slice-element-pointers
21 32 7 98
result 158
begin 121-self-referential-slice-literal
result 2
begin 124-fixed-array-pointer-cycles
result 9
begin 143-dynarray-runtime-capacity-and-resize
result 24
begin 235-run-pointer-dynarray-count-logical
result 0
begin 265-run-byte-slice-equality
result 0
begin 266-run-mutable-slice-literal
result 0
begin 267-run-fixed-array-equality
result 0

¬

¬


=== case: 183-implicit-array-slices ===
fill :: fn (values: []i32) {
    values[1] = 42
}

sum :: fn (values: []i32) -> i32 {
    return values[0] + values[1] + values[2]
}

main :: fn () -> i32 {
    values := [1, 2, 3]
    fill(values)
    return sum(values)
}
¬
46
¬

¬

¬

=== case: 056-run-llvm-nil-slice-equality-and-return ===
make :: fn (ok: bool) -> []u8 {
    on ok == no => return nil

    text: [2]u8 = ['o', 'k']
    return text[..]
}

main :: fn () -> i32 {
    empty: []u8 = nil
    text: [2]u8 = ['a', 'b']
    view := text[..]

    on empty != nil => return 1
    on nil != empty => return 2
    on view == nil => return 3
    on nil == view => return 4

    returned_empty := make(no)
    returned_full := make(yes)
    on returned_empty != nil => return 5
    on returned_full == nil => return 6

    return 0
}
¬
0
¬

¬

¬

=== case: 061-run-llvm-global-slice-literal ===
Location :: plex {
    description string
}

locs: []Location = [
    { description : "field" },
]

main :: fn () {
    player_loc: usize = 1
    for i, loc in locs {
        on {
            i == player_loc => prn("same")
            else => prn($"You go to {loc.description}.")
        }
    }
}
¬
0
¬
You go to field.

¬

¬

=== case: 069-run-llvm-dynarray-push ===
main :: fn () -> i32 {
    nums: [..]i32
    on nums.count != 0 => return 1
    on nums.capacity != 0 => return 2

    nums.push(4)
    nums.push(5)

    on nums.count != 2 => return 3
    on nums.capacity < 2 => return 4
    on nums[0] != 4 => return 5
    on nums[1] != 5 => return 6

    view := nums[..]
    on view.count != 2 => return 7
    on view[1] != 5 => return 8

    return 0
}
¬
0
¬

¬

¬

=== case: 070-run-llvm-dynarray-lifecycle ===
main :: fn () -> i32 {
    nums: [..]i32

    nums.reserve_to(8)
    on nums.count != 0 => return 1
    on nums.capacity < 8 => return 2

    nums.push(10)
    nums.push(20)
    nums.clear()
    on nums.count != 0 => return 3
    on nums.capacity < 8 => return 4

    nums.push(30)
    on nums[0] != 30 => return 5

    nums.free()
    on nums != nil => return 6
    on nums.count != 0 => return 7
    on nums.capacity != 0 => return 8

    return 0
}
¬
0
¬

¬

¬

=== case: 071-run-llvm-dynarray-remove ===
main :: fn () -> i32 {
    nums: [..]i32
    nums.push(1)
    nums.push(2)
    nums.push(3)
    nums.push(4)

    last := nums.pop()
    on last != 4 => return 1
    on nums.count != 3 => return 2

    nums.delete(1)
    on nums.count != 2 => return 3
    on nums[0] != 1 => return 4
    on nums[1] != 3 => return 5

    nums.push(5)
    nums.swap_delete(1)
    on nums.count != 2 => return 6
    on nums[0] != 1 => return 7
    on nums[1] != 5 => return 8

    return 0
}
¬
0
¬

¬

¬

=== case: 072-run-llvm-dynarray-append ===
main :: fn () -> i32 {
    nums: [..]i32
    nums.push(1)
    nums.push(2)

    extra := [3, 4, 5]
    nums.append(extra[..])

    on nums.count != 5 => return 1
    on nums[0] != 1 => return 2
    on nums[1] != 2 => return 3
    on nums[2] != 3 => return 4
    on nums[3] != 4 => return 5
    on nums[4] != 5 => return 6

    return 0
}
¬
0
¬

¬

¬

=== case: 073-run-llvm-dynarray-literal ===
main :: fn () -> i32 {
    nums: [..]i32 = [7, 8, 9]

    on nums.count != 3 => return 1
    on nums.capacity < 3 => return 2
    on nums[0] != 7 => return 3
    on nums[1] != 8 => return 4
    on nums[2] != 9 => return 5

    nums.free()
    return 0
}
¬
0
¬

¬

¬

=== case: 074-run-llvm-dynarray-min-capacity ===
main :: fn () -> i32 {
    nums: [4..]i32 = [1, 2]

    on nums.count != 2 => return 1
    on nums.capacity < 4 => return 2
    on nums[0] != 1 => return 3
    on nums[1] != 2 => return 4

    nums.free()
    return 0
}
¬
0
¬

¬

¬

=== case: 076-run-llvm-dynarray-resize ===
main :: fn () -> i32 {
    nums: [..]i32
    nums.push(4)
    nums.resize_to(3)

    on nums.count != 3 => return 1
    on nums.capacity < 3 => return 2
    on nums[0] != 4 => return 3
    on nums[1] != 0 => return 4
    on nums[2] != 0 => return 5

    nums.resize_undefined_to(1)
    on nums.count != 1 => return 6
    on nums[0] != 4 => return 7

    return 0
}
¬
0
¬

¬

¬

=== case: 077-run-llvm-dynarray-relative-capacity-and-size ===
main :: fn () -> i32 {
    nums: [..]i32

    nums.push(4)
    nums.reserve_extra(4)
    on nums.count != 1 => return 1
    on nums.capacity < 5 => return 2

    nums.extend(2)
    on nums.count != 3 => return 3
    on nums[0] != 4 => return 4
    on nums[1] != 0 => return 5
    on nums[2] != 0 => return 6

    nums.extend_undefined(2)
    on nums.count != 5 => return 7
    nums[3] = 8
    nums[4] = 9
    on nums[3] != 8 => return 8
    on nums[4] != 9 => return 9

    nums.resize_to(2)
    on nums.count != 2 => return 10

    nums.resize_undefined_to(1)
    on nums.count != 1 => return 11
    on nums[0] != 4 => return 12

    nums.free()
    return 0
}
¬
0
¬

¬

¬

=== case: 095-run-llvm-member-autoderef-and-array-count ===
use std.io

main :: fn () -> i32 {
    pair: (i32, i32) = (10, 20)
    pair_ptr := ^pair
    pair_ptr_ptr := ^pair_ptr

    values: [3]i32 = [1, 2, 3]
    values_ptr := ^values
    slice := values[..]
    slice_ptr := ^slice

    prn($"tuple {pair_ptr.0} {pair_ptr_ptr.1}")
    prn($"counts {values.count} {values_ptr.count} {slice.count} {slice_ptr.count}")
    prn($"data {values.data[0]} {values_ptr.data[1]} {[4, 5, 6].data[2]}")
    return pair_ptr.0 + pair_ptr_ptr.1 + values.data[0] +
        values_ptr.data[1] + [4, 5, 6].data[2]
}
¬
39
¬
tuple 10 20
counts 3 3 3 3
data 1 2 6

¬

¬

=== case: 210-run-fixed-array-implicit-slice ===
sum :: fn (bytes: []u8) -> i32 {
    return bytes.count.as(i32) + bytes[0].as(i32) + bytes[2].as(i32)
}

main :: fn () -> i32 {
    bytes: [3]u8 = [10, 20, 7]
    return sum(bytes)
}
¬
20
¬

¬

¬

=== case: 212-run-large-array-field-index ===
Doc :: plex {
    memory [65536]u8
}

main :: fn () -> i32 {
    doc: Doc
    doc.memory[65535] = 42
    return doc.memory[65535].as(i32)
}
¬
42
¬

¬

¬

=== case: 220-run-byte-arrays-to-string ===
--llvm
main :: fn () {
    signed : [5]i8
    signed[0] = 'H'
    signed[1] = 'e'
    signed[2] = 'l'
    signed[3] = 'l'
    signed[4] = 'o'

    unsigned : [5]u8
    unsigned[0] = 'W'
    unsigned[1] = 'o'
    unsigned[2] = 'r'
    unsigned[3] = 'l'
    unsigned[4] = 'd'

    dynamic : [..]i8
    dynamic.push('!')
    dynamic.push('!')

    prn(signed.as(string))
    prn(unsigned.as(string))
    prn(signed[1..4].as(string))
    prn(dynamic.as(string))
    ok : [2]u8 = ['O', 'K']
    prn(ok.as(string))

    dynamic.free()
}
¬
0
¬
Hello
World
ell
!!
OK

¬

¬

=== case: 330-runtime-fixed-arrays ===
fill :: fn [T] (n: i32, item: T) -> T {
    values: [n]T
    values[0] = item
    return values[0]
}
sum :: fn [T] (values: []T) -> T {
    total: T
    for value in values { total += value^ }
    return total
}
length :: fn (calls: ^i32) -> i32 {
    calls^ += 1
    return 3
}
fail :: fn () -> ?void { return nil }
cleanup :: fn (log: ^i32) -> ?void {
    n := 2
    values: [n]i32
    values[0] = 7
    defer log^ += values[0]
    undo log^ += values[0]
    fail()?
}
main :: fn () -> i32 {
    assert fill(2, 17) == 17
    calls: i32
    values: [length(^calls)]i32
    assert calls == 1
    assert values.count == 3
    assert values.size == 12
    assert values.bytes == 12
    values[0] = 5
    values[1] = 7
    assert values[2] == 0
    assert sum(values) == 12
    assert sum(values[..2]) == 12
    for value in values { value^ += 1 }
    assert sum(values) == 15
    n := 3
    other: [n + 1]u64
    n = 1
    assert other.count == 4
    assert other.size == 32
    empty: [n - 1]u8
    assert empty.count == 0
    assert empty.bytes == 0
    log: i32
    on cleanup(^log) => return 1
    assert log == 14
    for i := 0; i < 10; i += 1 {
        local: [i + 1]i32
        local[i] = i
        defer log += local[i]
        on i == 8 => break
        again
    }
    assert log == 50
    prn("Runtime fixed array lengths, views and cleanup passed")
    return 0
}

¬
0
¬
Runtime fixed array lengths, views and cleanup passed

¬

¬

=== case: 123-run-dynamic-array-interpolation ===
use std.io

main :: fn () {
    values : [..]u8
    values.push(1)
    values.push(2)
    prn($"values={values}")
    values.free()
}
¬
0
¬
values=[1, 2]

¬

¬


=== case: 124-run-dynamic-array-to-string-cast ===
use std.io

main :: fn () {
    name : [..]u8
    name.push('M')
    name.push('a')
    name.push('t')
    name.push('t')
    prn($"Hello, {name.as(string)}!")
    name.free()
}
¬
0
¬
Hello, Matt!

¬

¬


=== case: 136-run-llvm-index-assigned-local-array ===
use std.io

append_u64 :: fn (value: u64) {
    digits : [20]u8
    index := digits.count
    remaining := value

    on remaining == 0 => {
        index -= 1
        digits[index] = '0'
    } else {
        for remaining > 0 {
            index -= 1
            digits[index] = ('0'.as(u64) + (remaining % 10)).as(u8)
            remaining /= 10
        }
    }

    prn(digits[index..].as(string))
}

main :: fn () {
    append_u64(0)
    append_u64(12345)
}
¬
0
¬
0
12345

¬

¬


=== case: 325-runtime-array-i32-length ===
work :: fn (n: i32) -> i32 {
    values: [n]i32
    values[0] = 42
    return values[0]
}
main :: fn () -> i32 {
    return work(2) - 42
}
¬
0
¬

¬

¬
