use std.term

main :: fn () -> i32 {
    window: TermWindow
    rect: TermRect = term_rect(0, 0, 12, 4)

    term_window_init(^window, rect)
    ink := term_rgb(255, 255, 255)
    paper := term_rgb(0, 0, 0)
    term_window_paint_rect(^window, rect, ' '.as(u32), ink, paper)
    term_window_write(^window, 2, 1, "ok")

    first := window.cells[(1 * 12 + 2).as(usize)].ch
    second := window.cells[(1 * 12 + 3).as(usize)].ch

    term_window_done(^window)

    on first != 'o'.as(u32) => return 1
    on second != 'k'.as(u32) => return 2
    return 0
}
¬
0
¬
¬
¬
