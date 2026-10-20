-- tuples feature scenarios. Results and stdout are checked in order.
use std.io

-- 051-tuples
case_0_make_pair :: fn(a: i32, b: string) -> (i32, string) {
    return (a, b)
}

case_0_main :: fn () -> i32 {
    pair :: (7, "seven")
    single :: (pair.0 + 1,)
    from_fn :: case_0_make_pair(3, "three")
    nested :: (pair, single, yes)

    prn($"pair = {pair.0}, {pair.1}")
    prn($"pair tuple = {pair}")
    prn($"single = {single.0}")
    prn($"single tuple = {single}")
    prn($"from_fn = {from_fn.0}, {from_fn.1}")
    prn($"nested tuple = {nested}")

    return pair.0 + single.0 + from_fn.0
}

-- 059-destructuring-bindings
case_1_main :: fn () -> i32 {
    (a, b) := (2, 3)
    prn($"{a} {b}")
    (a, b) = (b, a)
    prn($"{a} {b}")
    (a, _) = (7, 8)
    prn($"{a} {b}")
    (c, d) :: (4, "four")
    prn($"{c} {d}")
    (e, f): (i32, string) = (5, "five")
    prn($"{e} {f}")
    return 0
}

-- 249-tuple-mutation
Case_2_Holder :: plex { pair (i32, (i32, i32)) }
case_2_change :: fn (pair: ^(i32, i32)) { pair.1 += 1 }
case_2_main :: fn () -> i32 {
    pair := (20, 22)
    pair.0 = 21
    pair.1 += 1
    case_2_change(^pair)
    assert pair.1 == 24
    pair.1 = 22
    nested := ((1, 2), 3)
    nested.0.1 *= 4
    assert nested.0.1 == 8
    holder := Case_2_Holder { pair: (1, (2, 3)) }
    holder.pair.1.0 += 2
    assert holder.pair.1.0 == 4
    return pair.0 + pair.1
}

-- 180-run-tuple-call-expression-items
case_3_value :: fn (x: i32) => x

case_3_pair :: fn () -> (i32, i32) {
    return (case_3_value(10), case_3_value(20))
}

case_3_main :: fn () {
    result := case_3_pair()
    prn($"{result.0}x{result.1}")
}

-- 183-run-return-untyped-tuple-coercion
case_4_choose :: fn (flag: bool) -> (u16, u16) {
    on flag => return (3, 5)
    return (0, 0)
}

case_4_main :: fn () -> i32 {
    result := case_4_choose(no)
    return (result.0 + result.1).as(i32)
}

-- 291-run-nested-tuple-items
case_5_main :: fn () {
    values := ((1, 2), (3, (4, 5)), 6)
    assert (values.0).0 == 1
    assert (values.0).1 == 2
    assert (values.1).0 == 3
    assert ((values.1).1).0 == 4
    assert ((values.1).1).1 == 5
    assert values.2 == 6
}

main :: fn () -> i32 {
    prn("begin 051-tuples")
    {
        result := case_0_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 18
    }
    prn("begin 059-destructuring-bindings")
    {
        result := case_1_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 249-tuple-mutation")
    {
        result := case_2_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 43
    }
    prn("begin 180-run-tuple-call-expression-items")
    {
        case_3_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 183-run-return-untyped-tuple-coercion")
    {
        result := case_4_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 291-run-nested-tuple-items")
    {
        case_5_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    return 0
}
¬
0
¬
begin 051-tuples
pair = 7, seven
pair tuple = (7, seven)
single = 8
single tuple = (8,)
from_fn = 3, three
nested tuple = ((7, seven), (8,), yes)
result 18
begin 059-destructuring-bindings
2 3
3 2
7 2
4 four
5 five
result 0
begin 249-tuple-mutation
result 43
begin 180-run-tuple-call-expression-items
10x20
result 0
begin 183-run-return-untyped-tuple-coercion
result 0
begin 291-run-nested-tuple-items
result 0

¬

¬


=== case: 093-run-llvm-nested-tuple-array ===
main :: fn () -> i32 {
    exits: [][](string, usize) = [
        [
            ("E", 1),
            ("W", 4),
        ],
        [("S", 2)],
    ]

    return (exits[0][1].1 + exits[1][0].1).as(i32)
}
¬
6
¬

¬

¬
