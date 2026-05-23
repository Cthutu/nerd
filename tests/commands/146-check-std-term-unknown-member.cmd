use std.term

main :: fn () {}

simulate :: fn (s: TermSimulate) {
    s.input
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
error: Unknown member `input` for `TermSimulate`
 --> 146-check-std-term-unknown-member.input.n:6:7
  |
4 | 
5 | simulate :: fn (s: TermSimulate) {
6 |     s.input
  |       ^^^^^ `TermSimulate` has no field or method named `input`
7 | }
  |
help: Use a field or method that exists on `TermSimulate`.
