use std.term

simulate :: fn (simulation: TermSimulate) {
on simulation.key_down(Escape)||simulation.key_down(Enter)||simulation.key_down(Q)=>term_done()
}
¬
use std.term

simulate :: fn (simulation: TermSimulate) {
    on simulation.key_down(Escape) ||
        simulation.key_down(Enter) ||
        simulation.key_down(Q) => term_done()
}
