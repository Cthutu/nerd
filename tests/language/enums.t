-- enums feature scenarios. Results and stdout are checked in order.
use std.io

-- 063-enum-unit-variants
Case_0_Colour :: enum { Red Green Blue }

case_0_score :: fn (colour: Case_0_Colour) -> i32 {
    return on colour {
        Red => 10
        Green => 20
        Blue => 30
    }
}

case_0_pick_shadowed :: fn () -> Case_0_Colour {
    Red: Case_0_Colour = Green
    picked: Case_0_Colour = Red
    return picked
}

case_0_main :: fn () -> i32 {
    red: Case_0_Colour = Red
    green: Case_0_Colour = Green
    blue := Case_0_Colour.Blue

    prn($"red {case_0_score(red)}")
    prn($"green {case_0_score(green)}")
    prn($"blue {case_0_score(blue)}")
    prn($"shadowed {case_0_score(case_0_pick_shadowed())}")
    prn($"total {case_0_score(red) + case_0_score(green) + case_0_score(blue)}")

    return 0
}

-- 065-enum-payloads
Case_1_Maybe :: enum { None Some(i32) Pair(i32, i32) Text(string) }

case_1_score :: fn (value: Case_1_Maybe) -> i32 {
    return on value {
        None => 0
        Some(x) => x
        Pair(left, right) => left + right
        Text(_) => 100
    }
}

case_1_main :: fn () -> i32 {
    a : Case_1_Maybe = None
    b : Case_1_Maybe = Some(5)
    c := Case_1_Maybe.Pair(10, 20)
    d : Case_1_Maybe = Text("hello")

    prn($"scores {case_1_score(a)} {case_1_score(b)} {case_1_score(c)} {case_1_score(d)}")

    return case_1_score(c)
}

-- 077-enum-discriminants
Case_2_Direction :: enum {
    NORTH,
    EAST,
    SOUTH = 10,
    WEST,
    COUNT
}

Case_2_Token :: enum {
    EOF = 0,
    IDENT,
    NUMBER = 10,
    STRING,
}

case_2_labels : [Case_2_Direction.COUNT]string = [
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"
]

case_2_describe_direction :: fn (direction: Case_2_Direction) -> string {
    return on direction {
        NORTH => "north"
        EAST => "east"
        SOUTH => "south"
        WEST => "west"
        COUNT => "count"
    }
}

case_2_describe_token :: fn (token: Case_2_Token) -> string {
    return on token {
        EOF => "eof"
        IDENT => "ident"
        NUMBER => "number"
        STRING => "string"
    }
}

case_2_main :: fn () -> i32 {
    south : Case_2_Direction = Case_2_Direction.SOUTH
    string_token : Case_2_Token = Case_2_Token.STRING

    prn(case_2_labels[0])
    prn(case_2_labels[11])
    prn(case_2_describe_direction(south))
    prn(case_2_describe_token(string_token))

    ok := on south {
        SOUTH => yes
        else => no
    } && on string_token {
        STRING => yes
        else => no
    }

    return on ok => 0 else 1
}

-- 083-explicit-return-enum-context
Case_3_Direction :: enum {
    NORTH
    EAST
    SOUTH
    WEST
    NO_DIRECTION
}

case_3_direction_from_word :: fn (word: string) -> Case_3_Direction {
    return on word {
        "north", "n" => NORTH
        "east",  "e" => EAST
        "south", "s" => SOUTH
        "west",  "w" => WEST
        else => NO_DIRECTION
    }
}

case_3_default_direction :: fn () -> Case_3_Direction {
    return on yes => NORTH else NO_DIRECTION
}

case_3_direction_name :: fn (direction: Case_3_Direction) -> string {
    return on direction {
        NORTH => "north"
        EAST => "east"
        SOUTH => "south"
        WEST => "west"
        else => "none"
    }
}

case_3_main :: fn () -> i32 {
    prn(case_3_direction_name(case_3_direction_from_word("n")))
    prn(case_3_direction_name(case_3_default_direction()))
    return 0
}

-- 143-enum-call-context
Case_4_Code :: enum {
    None
    Home
    End
}

case_4_score :: fn (code: Case_4_Code) -> i32 {
    on code == Home => return 10
    on code == End => return 20
    return 0
}

case_4_main :: fn () -> i32 {
    home: Case_4_Code = Home
    return case_4_score(home) + case_4_score(Case_4_Code.End)
}

-- 317-run-dynamic-enum-alignment
use std.term

case_5_main :: fn () {
    events: [..]TermInputEvent
    defer events.free()
    for i: u32 = 0; i < 40; i += 1 {
        events.push(TermInputEvent.Character(i + 65))
        assert (^events[0]).as(usize) % 16 == 0
    }
    events.reserve_to(100)
    assert (^events[0]).as(usize) % 16 == 0
    for i: usize = 0; i < events.count; i += 1 {
        on events[i] {
            Character(value) => { assert value == i.as(u32) + 65 }
            else => { assert no }
        }
    }
    reserved: [4..]TermInputEvent
    defer reserved.free()
    reserved.push(TermInputEvent.Character(99))
    assert (^reserved[0]).as(usize) % 16 == 0
}

main :: fn () -> i32 {
    prn("begin 063-enum-unit-variants")
    {
        result := case_0_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 065-enum-payloads")
    {
        result := case_1_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 30
    }
    prn("begin 077-enum-discriminants")
    {
        result := case_2_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 083-explicit-return-enum-context")
    {
        result := case_3_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 143-enum-call-context")
    {
        result := case_4_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 30
    }
    prn("begin 317-run-dynamic-enum-alignment")
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
begin 063-enum-unit-variants
red 10
green 20
blue 30
shadowed 10
total 60
result 0
begin 065-enum-payloads
scores 0 5 30 100
result 30
begin 077-enum-discriminants
0
11
south
string
result 0
begin 083-explicit-return-enum-context
north
north
result 0
begin 143-enum-call-context
result 30
begin 317-run-dynamic-enum-alignment
result 0

¬

¬


=== case: 127-run-enum-integer-casts ===
Key :: enum {
    None = 0
    Enter = 13
    LowerX = 120
    ArrowUp = 261
}

main :: fn () -> i32 {
    byte : u8 = 'x'
    from_byte := byte.as(Key)
    from_int := 261.as(Key)

    on from_byte != LowerX => return 1
    on from_int != ArrowUp => return 2
    on Key.Enter.as(u8) != 13 => return 3
    on Key.ArrowUp.as(u32) != 261 => return 4
    return 0
}
¬
0
¬

¬

¬

=== case: 217-run-enum-braced-payload-aliased-binders ===
use std.io

Event :: enum {
    Closed
    Resized {
        width u16
        height u16
    }
}

main :: fn () -> i32 {
    event := Event.Resized { width: 80, height: 25 }
    result := on event {
        Resized { width: observed_width, height: observed_height } =>
            observed_width.as(i32) + observed_height.as(i32)
        Closed => 0
    }
    prn($"{result}")
    return 0
}
¬
0
¬
105

¬

¬

=== case: 325-enum-final-return-fallthrough ===
Event :: enum {
    None
    Resized { width i32 }
    KeyPress { code i32 }
}
handle :: fn (event: Event) -> i32 {
    on event {
        None => {}
        Resized { width: _ } => {}
        KeyPress { code: 1 }, KeyPress { code: 2 } => { return 10 }
    }
    return 20
}
handle_void :: fn (event: Event, count: ^i32) {
    defer count^ += 100
    on event {
        None => {}
        Resized { width: _ } => { count^ += 1 }
        KeyPress { code: 1 }, KeyPress { code: 2 } => { return }
    }
    count^ += 10
}
main :: fn () -> i32 {
    on handle(Event.Resized { width: 5 }) != 20 => return 1
    on handle(Event.None) != 20 => return 2
    on handle(Event.KeyPress { code: 3 }) != 20 => return 3
    on handle(Event.KeyPress { code: 1 }) != 10 => return 4
    on handle(Event.KeyPress { code: 2 }) != 10 => return 5
    count: i32 = 0
    handle_void(Event.Resized { width: 5 }, ^count)
    on count != 111 => return 6
    count = 0
    handle_void(Event.KeyPress { code: 1 }, ^count)
    on count != 100 => return 7
    count = 0
    handle_void(Event.KeyPress { code: 3 }, ^count)
    on count != 110 => return 8
    prn("Enum branches continue correctly")
    return 0
}
¬
0
¬
Enum branches continue correctly

¬

¬

=== case: 357-enum-integer-contexts ===
other :: use enum_integer_contexts

Colour :: enum { Red Green = 4 Blue }
Large :: enum { Value = 4294967295 }
green :: Colour.Green
number: i32: Colour.Green
byte: u8: green
imported: u16: other.Colour.Green
imported_alias: i32: other.green

take_integer :: fn (value: i32) => value
take_colour :: fn (value: Colour) => value
text_length :: fn (value: string) => value.count.as(i32)
numeric_or_text :: fn { take_integer text_length }
enum_or_numeric :: fn { take_integer take_colour }
integer_result :: fn () -> i32 { return Colour.Green }

main :: fn () {
    colour := Colour.Green
    assert take_colour(colour) == Colour.Green
    assert take_colour(green) == Colour.Green
    imported_colour := other.Colour.Green
    assert imported_colour == other.Colour.Green
    assert number == 4 && byte == 4
    assert imported == 4 && imported_alias == 4
    i: i32 = Colour.Green
    small: u8 = Colour.Green
    alias :: green
    from_alias: u16 = alias
    assert i == 4 && small == 4 && from_alias == 4
    assert take_integer(Colour.Green) == 4
    assert take_integer(other.Colour.Green) == 4
    assert numeric_or_text(Colour.Green) == 4
    assert numeric_or_text(green) == 4
    assert enum_or_numeric(Colour.Green) == Colour.Green
    assert take_colour(alias) == Colour.Green
    assert integer_result() == 4
    sum: i32 = Colour.Green + 38
    assert sum == 42
    float: f32 = Colour.Green
    assert float == 4.0
    optional: ?i32 = Colour.Green
    assert (optional \ 0) == 4
    wide: u32 = Large.Value
    assert wide == 4294967295
    assert colour == Colour.Green
    prn("enum integer contexts passed")
}
¬
0
¬
enum integer contexts passed
¬

¬

=== case: 137-run-llvm-dynarray-global-enum-stress ===
Key :: enum { A B }
Event :: enum { None KeyDown(Key) Resize(u32, u32) }

events: [..]Event

push_event :: fn (event: Event) {
    events.push(event)
}

poll :: fn () -> Event {
    on events.count == 0 => return Event.None
    event := events[0]
    events.delete(0)
    return event
}

main :: fn () -> i32 {
    for i: u32 = 0; i < 100000; i += 1 {
        push_event(Event.Resize(i, i + 1))
        event := poll()
        on event {
            Resize(width, height) => {
                on width != i => return 1
                on height != i + 1 => return 2
            }
            else => return 3
        }
    }

    on events.count != 0 => return 4
    events.free()
    return 0
}
¬
0
¬

¬

¬
