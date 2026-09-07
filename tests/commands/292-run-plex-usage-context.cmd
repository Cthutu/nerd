State :: plex {
    background_colour (f32, f32, f32)
}
Outer :: plex { state State }
Value :: union { number f32 bits u32 }
returned :: fn () -> State {
    value := { background_colour: (0.25, 0.5, 0.75) }
    return value
}
main :: fn () {
    draw_state := { background_colour: (0.25, 0.5, 0.75) }
    assert draw(^draw_state) == 0.25
    assert draw_state.background_colour.0.size == f32.size

    red := 0.5
    initial := { background_colour: (red, 0.25, 0.75) }
    alias := initial
    pointer := ^alias
    assert draw(pointer) == 0.5
    assert red.size == f32.size

    nested := { state: { background_colour: (0.75, 0.5, 0.25) } }
    assert outer(nested) == 0.75

    assigned := { background_colour: (0.25, 0.5, 0.75) }
    destination: State
    destination = assigned
    assert draw(^destination) == 0.25
    result := returned()
    assert draw(^result) == 0.25

    union_value := { number: 0.5 }
    assert read_union(^union_value) == 0.5
}
draw :: fn (state: ^State) -> f32 { return state.background_colour.0 }
outer :: fn (value: Outer) -> f32 { return value.state.background_colour.0 }
read_union :: fn (value: ^Value) -> f32 { return value.number }
¬
0
¬

¬
delete
