use std.term

main :: fn () -> i32 {
    window: TermWindow
    rect := TermRect { x: 0, y: 0, width: 12, height: 4 }
    other := TermRect { x: 2, y: 1, width: 4, height: 2 }
    overlap := rect.intersection(other)
    reversed := TermRect { x: 6, y: 4, width: 0, height: 0 }.to_point(3, 2)
    combined := other.union_with(TermRect { x: 8, y: 3, width: 2, height: 2 })
    clip := other.clip(TermRect { x: 3, y: 0, width: 2, height: 3 })

    on rect.is_empty() => return 1
    on overlap.x != 2 || overlap.y != 1 => return 2
    on overlap.width != 4 || overlap.height != 2 => return 3
    on !overlap.equals(other) => return 4
    on !rect.contains(11, 3) => return 5
    on rect.contains(12, 3) => return 6
    on !rect.overlaps(other) => return 7
    on other.overlaps(TermRect { x: 20, y: 20, width: 2, height: 2 }) => return 8
    on reversed.x != 3 || reversed.y != 2 => return 9
    on reversed.width != 4 || reversed.height != 3 => return 10
    on combined.x != 2 || combined.y != 1 => return 11
    on combined.width != 8 || combined.height != 4 => return 12
    on !clip.ok => return 13
    on clip.clipped.x != 3 || clip.clipped.y != 1 => return 14
    on clip.clipped.width != 2 || clip.clipped.height != 2 => return 15
    on clip.local.x != 1 || clip.local.y != 0 => return 16

    term_window_init(^window, rect)
    ink := term_rgb(255, 255, 255)
    paper := term_rgb(0, 0, 0)
    term_window_paint_rect(^window, rect, ' '.as(u32), ink, paper)
    term_window_write(^window, 2, 1, "ok")

    first_cell := window.cells[(1 * 12 + 2).as(usize)]
    second_cell := window.cells[(1 * 12 + 3).as(usize)]

    term_window_done(^window)

    on first_cell.ch != 'o'.as(u32) => return 17
    on second_cell.ch != 'k'.as(u32) => return 18
    on first_cell.ink != ink || second_cell.ink != ink => return 19
    on first_cell.paper != paper || second_cell.paper != paper => return 20
    return 0
}
¬
0
¬
¬
¬
