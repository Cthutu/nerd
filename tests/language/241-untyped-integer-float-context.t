-- Untyped integer bindings retain their type and adopt float contexts per use.
imported :: use tests.mods.untyped_float_context
answer :: 42
alias :: answer
float_answer : f32 : alias
double_answer : f64 : answer
negative_answer : f32 : -answer
literal_answer : f64 : 42
sum_answer : f32 : answer + 1
large_answer : f64 : 4_294_967_296
global_answer : f32 = answer

identity :: fn (value: f32) -> f32 { return value }
from_binding :: fn () -> f64 { return alias }

main :: fn () {
    local : f32 = answer
    assert local == 42.0
    assert float_answer == 42.0
    assert double_answer == 42.0
    assert negative_answer == -42.0
    assert literal_answer == 42.0
    assert sum_answer == 43.0
    assert large_answer == 4_294_967_296.0
    assert global_answer == 42.0
    assert identity(imported.answer) == 42.0
    assert identity(answer) == 42.0
    assert identity(42) == 42.0
    assert from_binding() == 42.0
    assert float_answer + answer == 84.0
    assert answer + double_answer == 84.0
    assert answer == float_answer
    assert double_answer == answer
    assert answer < double_answer + 1.0
    assert double_answer + 1.0 > answer
    assert answer + 1 == 43
    prn("untyped integer float contexts passed")
}
¬
0
¬
untyped integer float contexts passed
¬
const value.0: untyped integer = untyped integer 42
¬
