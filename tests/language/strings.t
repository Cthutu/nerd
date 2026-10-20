-- strings feature scenarios. Results and stdout are checked in order.
use std.io

-- 011-adjacent-strings
-- Concatenates continuation string literals at compile time.
case_0_main :: fn () {
    prn("Hello, " +"world!")
}

-- 019-interpolated-strings
-- Builds an interpolated string from a constant and a variable.
case_1_name :: "world"
case_1_count := 3

case_1_check_escape :: fn (escape: string) -> bool {
    on escape.count != 4 => return no
    on escape.data[0] != 27 => return no
    on escape.data[1] != '[' => return no
    on escape.data[2] != '3' => return no
    on escape.data[3] != 'm' => return no
    return yes
}

case_1_main :: fn () {
    prn($"Hello, {case_1_name}! count={case_1_count}")
    on !case_1_check_escape($"\x1b[{case_1_count}m") => return
}

-- 034-local-const-interpolation
case_2_main :: fn () {
  i :: 2
  f :: 3.14
  s :: "Hello, world!"

  prn($"{s}  i = {i} and f = {f}!")
}

-- 039-5-prints
case_3_main :: fn () {
    count := 0

    for {
        on count >= 5 => return

        prn($"Iteration {count}")
        count += 1
    }
}

-- 125-top-level-interpolated-strings
case_4_name :: "world"
case_4_count :: 40 + 2
case_4_truth :: yes
case_4_greeting :: $"Hello, {case_4_name}! count={case_4_count}, ok={case_4_truth}"

case_4_make :: fn () -> string {
    return $"constant {case_4_name} {case_4_count}"
}

case_4_main :: fn () => ((case_4_greeting != "Hello, world! count=42, ok=yes") ||
                  (case_4_make() != "constant world 42")).as(i32)

-- 136-interpolated-assert-messages
-- Checks assertion messages can be interpolated strings.
case_5_main :: fn () -> i32 {
    count := 3
    assert count == 3, $"count is {count}"
    return 0
}

-- 154-string-continuations
case_6_main :: fn () -> i32 {
    name := "Matt"
    plain := "Hello, " +"world!"

    on plain != "Hello, world!" => return 1
    prn($"Hello {name}, " +"again {name}!")
    return 0
}

-- 115-run-interpolated-string-brace-escapes
case_7_main :: fn () {
    value := 42
    prn($"literal \{value\} actual {value}")
}

-- 187-run-triple-string
case_8_main :: fn () -> i32 {
    text := """
        alpha
          beta
        gamma
        """

    expected := "alpha\n  beta\ngamma\n"
    on text != expected => return 1

    compact := """one
    two"""
    on compact != "one\n    two" => return 2

    escaped := """
        quote: \"""
        slash: \\
        """
    on escaped != "quote: \"\"\"\nslash: \\\n" => return 3

    return 0
}

-- 273-run-string-parameter-index
case_9_first :: fn (value: string) -> u8 {
    return value[0]
}

case_9_main :: fn () -> i32 {
    assert case_9_first("Nerd") == 'N'
    return 0
}

main :: fn () -> i32 {
    prn("begin 011-adjacent-strings")
    {
        case_0_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 019-interpolated-strings")
    {
        case_1_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 034-local-const-interpolation")
    {
        case_2_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 039-5-prints")
    {
        case_3_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 125-top-level-interpolated-strings")
    {
        result := case_4_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 136-interpolated-assert-messages")
    {
        result := case_5_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 154-string-continuations")
    {
        result := case_6_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 115-run-interpolated-string-brace-escapes")
    {
        case_7_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 187-run-triple-string")
    {
        result := case_8_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 273-run-string-parameter-index")
    {
        result := case_9_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    return 0
}
¬
0
¬
begin 011-adjacent-strings
Hello, world!
result 0
begin 019-interpolated-strings
Hello, world! count=3
result 0
begin 034-local-const-interpolation
Hello, world!  i = 2 and f = 3.14!
result 0
begin 039-5-prints
Iteration 0
Iteration 1
Iteration 2
Iteration 3
Iteration 4
result 0
begin 125-top-level-interpolated-strings
result 0
begin 136-interpolated-assert-messages
result 0
begin 154-string-continuations
Hello Matt, again Matt!
result 0
begin 115-run-interpolated-string-brace-escapes
literal {value} actual 42
result 0
begin 187-run-triple-string
result 0
begin 273-run-string-parameter-index
result 0

¬

¬


=== case: 023-run-llvm-adjacent-strings ===
use std.io

main :: fn () {
    prn("Hello, " +"world!")
}
¬
0
¬
Hello, world!

¬

¬

=== case: 025-run-llvm-interpolation ===
use std.io

name :: "world"
count := 3

main :: fn () {
    prn($"Hello, {name}! count={count}")
}
¬
0
¬
Hello, world! count=3

¬

¬

=== case: 029-run-llvm-nested-interpolation ===
use std.io

warm :: "warm"
cool :: "cool"
unknown :: "unknown"

classify :: fn (choice: string) -> i32 {
    return on choice {
        "red", "green" => 1
        "blue" => 2
        else => 3
    }
}

label :: fn (choice: string) -> string {
    return on choice {
        "red" => warm
        "blue" => cool
        else => unknown
    }
}

main :: fn () {
    prn($"red: {classify("red")} {label("red")}")
    prn($"blue: {classify("blue")} {label("blue")}")
    return classify("blue")
}
¬
2
¬
red: 1 warm
blue: 2 cool

¬

¬

=== case: 217-run-string-bytes ===
use std.io

main :: fn () -> i32 {
    text :: "hello"
    ell := text[1..4]
    prn($"text={text.bytes} ell={ell.bytes}")
    on text.bytes != text.count => return 1
    on ell.bytes != ell.count => return 2
    return (text.bytes + ell.bytes).as(i32)
}
¬
8
¬
text=5 ell=3

¬

¬

=== case: 145-run-release-runtime-u8-interpolation ===
-- test-release
use test.runtime_helper_u8

main :: fn () {
    print_rgb_bytes(0xffebb4)
}
¬
0
¬
255 235 180

¬

¬


=== case: continued-literal-three-parts ===
message :: "global "
    +"continued "
    +"literal"
main :: fn () {
    prn("one "
        +"two "
        +"three")
    prn(message)
    prn("four "
        +"part "
        +"continued "
        +"literal")
}
¬
0
¬
one two three
global continued literal
four part continued literal
¬

¬
