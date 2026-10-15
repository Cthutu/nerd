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
delete
¬
