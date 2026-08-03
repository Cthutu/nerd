Value :: plex {
    number i32
}

impl Value {
    increment :: fn (value: ^Self) {
        value.number += 1
    }
}

make_value :: fn () -> ?Value {
    return Value { number: 40 }
}

main :: fn () -> i32 {
    result := 0
    on make_value() => [value] {
        value.increment()
        result = value.number
    }
    return result - 41
}
¬
0
¬

¬

¬
