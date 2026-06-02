Key :: enum {
    A = 65
}

Maybe :: enum {
    None
    Some(Key)
}

Event :: enum {
    None
    KeyPress(Key)
}

main :: fn () -> i32 {
    maybe := Maybe.Some(Key.A)
    event := on maybe {
        Some(code) => Event.KeyPress(code)
        None => Event.None
    }

    return on event {
        KeyPress(code) => code.as(i32) - 65
        None => 1
    }
}
¬
0
¬
¬
delete
¬
