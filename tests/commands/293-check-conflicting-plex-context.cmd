First :: plex { value f32 }
Second :: plex { value f64 }
first :: fn (_value: ^First) {}
second :: fn (_value: ^Second) {}
main :: fn () {
    value := { value: 0.25 }
    first(^value)
    second(^value)
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
error: Type mismatch: expected `First`, found `Second`
 --> 293-check-conflicting-plex-context.input.n:6:14
  |
4 | second :: fn (_value: ^Second) {}
5 | main :: fn () {
6 |     value := { value: 0.25 }
  |              ^ This expression has type `Second`
7 |     first(^value)
8 |     second(^value)
  |
help: Change the expression or annotation so both sides use the same type.
