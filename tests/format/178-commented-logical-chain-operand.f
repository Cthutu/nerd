use std.term

quit :: fn (input: TermInput) {
    on input.key_pressed(Escape) ||
        -- input.key_pressed(Enter) ||
        input.key_pressed(Q) => term_done()
}
¬
use std.term

quit :: fn (input: TermInput) {
    on input.key_pressed(Escape) ||
        -- input.key_pressed(Enter) ||
        input.key_pressed(Q) => term_done()
}
