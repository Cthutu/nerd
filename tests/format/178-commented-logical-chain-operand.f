use std.term

quit :: fn (input: TermInput) {
    on term_key_pressed(input.keyboard, Escape) ||
        -- term_key_pressed(input.keyboard, LowerQ) ||
        term_key_pressed(input.keyboard, Q) => term_done()
}
¬
use std.term

quit :: fn (input: TermInput) {
    on term_key_pressed(input.keyboard, Escape) ||
    -- term_key_pressed(input.keyboard, LowerQ) ||
    term_key_pressed(input.keyboard, Q) => term_done()
}
