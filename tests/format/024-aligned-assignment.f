-- This test checks that the formatter aligns variable declarations and
-- assignments properly.

test :: fn () {
    one_var:=1

    single_const::1

    after_blank: i32 = 2
}

main :: fn () {
    answer::3.14
    typed:f64:2.0
    ratio: f64 = 3.14 + 2.0 * 3.0
    s :string = "hello"
    flags: u32 = 7 | 2 ^ 1 & 3
    same := ratio >= 7.5 && flags != 0
    return on same => "ok" else "bad"
}

bind_values :: fn () {
    first::1
    longer::2
    typed:i32:3
}

single_mixed :: fn () {
    one::1
    two:=2
    three:u32=3
}

long_values :: fn () {
    long_const: string: "this const string is deliberately long enough to wrap after the second colon"
    short_const: string: "ok"
    long_name: string = "this string is deliberately long enough to wrap after the equals"
    short: string = "ok"
}
¬
-- This test checks that the formatter aligns variable declarations and
-- assignments properly.

test :: fn () {
    one_var := 1

    single_const :: 1

    after_blank : i32 = 2
}

main :: fn () {
    answer :: 3.14
    typed : f64 : 2.0
    ratio : f64    = 3.14 + 2.0 * 3.0
    s     : string = "hello"
    flags : u32    = 7 | 2 ^ 1 & 3

    same := ratio >= 7.5 && flags != 0
    return on same => "ok" else "bad"
}

bind_values :: fn () {
    first :: 1
    longer :: 2
    typed : i32 : 3
}

single_mixed :: fn () {
    one :: 1
    two := 2
    three : u32 = 3
}

long_values :: fn () {
    long_const  : string :
        "this const string is deliberately long enough to wrap after the second colon"
    short_const : string : "ok"

    long_name : string =
        "this string is deliberately long enough to wrap after the equals"
    short     : string = "ok"
}
