use std.term

simulate :: fn (s: TermSimulate) {
on term_key_down(s.keyboard, Escape)||term_key_down(s.keyboard, LowerQ)||term_key_down(s.keyboard, Q)=>term_done()
}
¬
use std.term

simulate :: fn (s: TermSimulate) {
    on term_key_down(s.keyboard, Escape) ||
        term_key_down(s.keyboard, LowerQ) ||
        term_key_down(s.keyboard, Q) => term_done()
}
