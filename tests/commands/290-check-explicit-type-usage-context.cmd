single :: fn (_value: f32) {}

main :: fn () {
    colour : (f64, f64) = (0.25, 0.5)
    single(colour.0)
}
¬
1
¬

¬
delete
¬

¬
check
¬
error: Type mismatch: expected `f32`, found `f64`
 --> 290-check-explicit-type-usage-context.input.n:5:19
  |
3 | main :: fn () {
4 |     colour : (f64, f64) = (0.25, 0.5)
5 |     single(colour.0)
  |                   ^ This expression has type `f64`
6 | }
  |
help: Change the expression or annotation so both sides use the same type.
