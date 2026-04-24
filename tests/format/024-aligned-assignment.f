-- This test checks that the formatter aligns variable declarations and
-- assignments properly.

test :: fn () {
    one_var:=1

    after_blank: i32 = 2
}

main :: fn () {
    ratio: f64 = 1.5 + 2.0 * 3.0
    s :string = "hello"
    flags: u32 = 7 | 2 ^ 1 & 3
    same := ratio >= 7.5 && flags != 0
    return on same => "ok" else "bad"
}

long_values :: fn () {
    long_name: string = "this string is deliberately long enough to wrap after the equals"
    short: string = "ok"
}
¬
-- This test checks that the formatter aligns variable declarations and
-- assignments properly.

test :: fn () {
    one_var := 1

    after_blank : i32 = 2
}

main :: fn () {
    ratio : f64    = 1.5 + 2.0 * 3.0
    s     : string = "hello"
    flags : u32    = 7 | 2 ^ 1 & 3
    same  :        = ratio >= 7.5 && flags != 0

    return on same => "ok" else "bad"
}

long_values :: fn () {
    long_name : string =
        "this string is deliberately long enough to wrap after the equals"
    short     : string = "ok"
}
