single :: fn (value: f32) -> f32 { return value }
double :: fn (value: f64) -> f64 { return value }
pair :: fn (value: (f32, f64)) -> f32 { return value.0 }
pointed :: fn (value: ^f32) -> f32 { return value^ }

returned :: fn () -> f32 {
    values := (0.25, 0.5)
    return values.0
}

with_parameter :: fn (amount: f32) -> f32 {
    initial := 0.25
    alias := initial
    output := alias + amount
    return output
}

main :: fn () {
    scalar := 0.25
    assert single(scalar) == 0.25
    assert scalar.size == f32.size

    colour := (0.25, 0.5, 0.75, 1.0)
    alias := colour.0
    assert single(alias) == 0.25
    assert double(colour.1) == 0.5
    assert single(colour.2 + 0.25) == 1.0
    assert colour.0.size == f32.size
    assert colour.1.size == f64.size
    assert colour.2.size == f32.size
    assert colour.3.size == f64.size
    colour = (0.5, 0.75, 0.25, 1.0)
    assert single(colour.0) == 0.5

    nested := ((0.25, 0.5), 0.75)
    assert single((nested.0).1) == 0.5
    assert (nested.0).1.size == f32.size
    assert (nested.0).0.size == f64.size

    whole := (0.25, 0.5)
    assert pair(whole) == 0.25
    assert whole.0.size == f32.size
    assert whole.1.size == f64.size

    assigned := 0.25
    destination : f32
    destination = assigned
    assert destination == 0.25
    assert assigned.size == f32.size
    assert returned() == 0.25
    assert with_parameter(0.5) == 0.75

    mixed := (yes, 0.25, "colour")
    assert single(mixed.1) == 0.25
    assert mixed.0
    assert mixed.2 == "colour"

    integer := 42
    wide: u64 = integer
    assert wide == 42
    assert integer.size == u64.size

    addressed := 0.5
    assert pointed(^addressed) == 0.5
    assert addressed.size == f32.size

    casted := 0.25
    assert single(casted.as(f32)) == 0.25
    assert casted.size == f64.size

    compared := 0.5
    reference: f32 = 0.5
    assert compared == reference
    assert compared.size == f32.size
}
¬
0
¬

¬
delete
