-- test-platform: linux

Cell :: plex {
    ink   u32
    paper u32
    ch    u32
}

Rect :: plex {
    x      u16
    y      u16
    width  u16
    height u16
}

Clip :: plex {
    ok      bool
    clipped Rect
    local   Rect
}

Window :: plex {
    rect  Rect
    cells [48]Cell
}

impl Rect {
    to_point :: fn (rect: Self, x1: u16, y1: u16) -> Self {
        x0 := on rect.x < x1 => rect.x else x1
        y0 := on rect.y < y1 => rect.y else y1
        max_x := on rect.x > x1 => rect.x else x1
        max_y := on rect.y > y1 => rect.y else y1
        return Rect {
            x: x0,
            y: y0,
            width: (max_x - x0 + 1).as(u16),
            height: (max_y - y0 + 1).as(u16),
        }
    }

    union_with :: fn (a: Self, b: Self) -> Self {
        x0 := on a.x < b.x => a.x else b.x
        y0 := on a.y < b.y => a.y else b.y
        ax1 := a.x.as(u32) + a.width.as(u32)
        ay1 := a.y.as(u32) + a.height.as(u32)
        bx1 := b.x.as(u32) + b.width.as(u32)
        by1 := b.y.as(u32) + b.height.as(u32)
        x1 := on ax1 > bx1 => ax1 else bx1
        y1 := on ay1 > by1 => ay1 else by1
        return Rect { x: x0, y: y0, width: (x1 - x0.as(u32)).as(u16), height: (y1 - y0.as(u32)).as(u16) }
    }

    is_empty :: fn (rect: Self) -> bool {
        return rect.width == 0 || rect.height == 0
    }

    equals :: fn (a: Self, b: Self) -> bool {
        return a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height
    }

    contains :: fn (rect: Self, x: u16, y: u16) -> bool {
        return x >= rect.x && x < rect.x + rect.width && y >= rect.y && y < rect.y + rect.height
    }

    overlaps :: fn (a: Self, b: Self) -> bool {
        return a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height && a.y + a.height > b.y
    }

    intersection :: fn (a: Self, b: Self) -> Self {
        ax1 := a.x.as(u32) + a.width.as(u32)
        ay1 := a.y.as(u32) + a.height.as(u32)
        bx1 := b.x.as(u32) + b.width.as(u32)
        by1 := b.y.as(u32) + b.height.as(u32)
        x0 := on a.x > b.x => a.x else b.x
        y0 := on a.y > b.y => a.y else b.y
        x1 := on ax1 < bx1 => ax1 else bx1
        y1 := on ay1 < by1 => ay1 else by1
        on x1 <= x0.as(u32) || y1 <= y0.as(u32) => return Rect { x: 0, y: 0, width: 0, height: 0 }
        return Rect { x: x0, y: y0, width: (x1 - x0.as(u32)).as(u16), height: (y1 - y0.as(u32)).as(u16) }
    }

    clip :: fn (a: Self, b: Self) -> Clip {
        clipped := a.intersection(b)
        on clipped.width == 0 || clipped.height == 0 => {
            empty := Rect { x: 0, y: 0, width: 0, height: 0 }
            failed: Clip = Clip { ok: no, clipped: empty, local: empty }
            return failed
        }
        local := Rect { x: (clipped.x - a.x).as(u16), y: (clipped.y - a.y).as(u16), width: clipped.width, height: clipped.height }
        ok: Clip = Clip { ok: yes, clipped: clipped, local: local }
        return ok
    }
}

paint_rect :: fn (window: ^Window, rect: Rect, ch: u32, ink: u32, paper: u32) {
    for y: u16 = 0; y < rect.height; y += 1 {
        for x: u16 = 0; x < rect.width; x += 1 {
            index := ((rect.y + y).as(usize) * window.rect.width.as(usize)) + (rect.x + x).as(usize)
            window.cells[index] = Cell { ink: ink, paper: paper, ch: ch }
        }
    }
}

write_text :: fn (window: ^Window, x: u16, y: u16, text: string, ink: u32, paper: u32) {
    for i: usize = 0; i < text.count; i += 1 {
        index := y.as(usize) * window.rect.width.as(usize) + x.as(usize) + i
        window.cells[index] = Cell { ink: ink, paper: paper, ch: text.data[i].as(u32) }
    }
}

skip_zeroes :: fn (values: []u8) -> usize {
    count: usize = 0
    for i: usize = 0; i < values.count; i += 1 {
        on values[i] == 0 => again
        count += 1
    }
    return count
}

main :: fn () -> i32 {
    rect := Rect { x: 0, y: 0, width: 12, height: 4 }
    other := Rect { x: 2, y: 1, width: 4, height: 2 }
    overlap := rect.intersection(other)
    reversed := Rect { x: 6, y: 4, width: 0, height: 0 }.to_point(3, 2)
    combined := other.union_with(Rect { x: 8, y: 3, width: 2, height: 2 })
    clip := other.clip(Rect { x: 3, y: 0, width: 2, height: 3 })

    on rect.is_empty() => return 1
    on overlap.x != 2 || overlap.y != 1 => return 2
    on overlap.width != 4 || overlap.height != 2 => return 3
    on !overlap.equals(other) => return 4
    on !rect.contains(11, 3) => return 5
    on rect.contains(12, 3) => return 6
    on !rect.overlaps(other) => return 7
    on other.overlaps(Rect { x: 20, y: 20, width: 2, height: 2 }) => return 8
    on reversed.x != 3 || reversed.y != 2 => return 9
    on reversed.width != 4 || reversed.height != 3 => return 10
    on combined.x != 2 || combined.y != 1 => return 11
    on combined.width != 8 || combined.height != 4 => return 12
    on !clip.ok => return 13
    on clip.clipped.x != 3 || clip.clipped.y != 1 => return 14
    on clip.clipped.width != 2 || clip.clipped.height != 2 => return 15
    on clip.local.x != 1 || clip.local.y != 0 => return 16

    window: Window
    window.rect = rect
    ink := 0x00ffffff.as(u32)
    paper := 0.as(u32)
    paint_rect(^window, rect, ' '.as(u32), ink, paper)
    write_text(^window, 2, 1, "ok", ink, paper)

    first_cell := window.cells[(1 * 12 + 2).as(usize)]
    second_cell := window.cells[(1 * 12 + 3).as(usize)]

    on first_cell.ch != 'o'.as(u32) => return 17
    on second_cell.ch != 'k'.as(u32) => return 18
    on first_cell.ink != ink || second_cell.ink != ink => return 19
    on first_cell.paper != paper || second_cell.paper != paper => return 20
    bytes: [4]u8 = [0, 1, 0, 2]
    on skip_zeroes(bytes[..]) != 2 => return 21

    return 0
}
¬
0
¬

¬

¬
