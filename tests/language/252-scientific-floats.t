identity :: fn [T] (value: T) -> T { return value }
main :: fn () {
    value: f64 = 1e3
    small: f64 = 1e-3
    single: f32 = 1E+3
    assert value == 1000.0 && single == 1000.0
    assert small > 0.0009 && small < 0.0011
    assert 1.0e3 == value
    assert identity(2e2) == 200.0
    assert 1_000e-1 == 100.0 && 1e1_0 == 10000000000.0
    assert 123456789012345678901234567890e-29 > 1.0
    assert 0x1e3 == 483
    values := [1, 2, 3]
    assert values[1] == 2
    prn("Scientific floats passed")
}
¬
0
¬
Scientific floats passed
¬
¬
