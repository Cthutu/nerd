use std.io
use std.term
use std.time

simulate :: fn (_sim: TermSimulate) {}

present :: fn (_present: TermPresent) {}

main :: fn () -> i32 {
    term_hook_simulation(from_ms(16), simulate)
    term_hook_presentation(from_ms(100), present)
    prn("default nil pointer ok")
    return 0
}
¬
0
¬
default nil pointer ok

¬
delete
¬
--llvm
