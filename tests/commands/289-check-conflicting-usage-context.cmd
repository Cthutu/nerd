single :: fn (_value: f32) {}
double :: fn (_value: f64) {}
-- Both calls constrain the same tuple field.
main :: fn () {
    colour := (0.25, 0.5)
    alias := colour.0
    single(alias)
    double(colour.0)
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
 --> 289-check-conflicting-usage-context.input.n:5:16
  |
3 | -- Both calls constrain the same tuple field.
4 | main :: fn () {
5 |     colour := (0.25, 0.5)
  |                ^^^^ This expression has type `f64`
6 |     alias := colour.0
7 |     single(alias)
  |
help: Change the expression or annotation so both sides use the same type.
