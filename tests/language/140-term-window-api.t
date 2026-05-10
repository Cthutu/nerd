-- test-platform: linux

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
hir 0
module module.0(140-term-window-api.input)
import module.1(std.term)
import import.0 Term from module.1(std.term).decl.0: Term
import import.1 TermEvent from module.1(std.term).decl.1: TermEvent
import import.2 TermKeyEvent from module.1(std.term).decl.2: TermKeyEvent
import import.3 TermMouseEvent from module.1(std.term).decl.3: TermMouseEvent
import import.4 TermKeyCode from module.1(std.term).decl.4: TermKeyCode
import import.5 TERM_KEYMOD_CTRL from module.1(std.term).decl.5: untyped integer
import import.6 TERM_KEYMOD_ALT from module.1(std.term).decl.6: untyped integer
import import.7 TERM_KEYMOD_SHIFT from module.1(std.term).decl.7: untyped integer
import import.8 TermRect from module.1(std.term).decl.8: TermRect
import import.9 TermRectClip from module.1(std.term).decl.9: TermRectClip
import import.10 TermCell from module.1(std.term).decl.10: TermCell
import import.11 TermWindow from module.1(std.term).decl.11: TermWindow
import import.12 term_size from module.1(std.term).decl.12: fn () -> (i32, i32)
import import.13 __impl_131_to_point from module.1(std.term).decl.13: fn (TermRect, u16, u16) -> TermRect
import import.14 __impl_131_union_with from module.1(std.term).decl.14: fn (TermRect, TermRect) -> TermRect
import import.15 __impl_131_is_empty from module.1(std.term).decl.15: fn (TermRect) -> bool
import import.16 __impl_131_equals from module.1(std.term).decl.16: fn (TermRect, TermRect) -> bool
import import.17 __impl_131_contains from module.1(std.term).decl.17: fn (TermRect, u16, u16) -> bool
import import.18 __impl_131_overlaps from module.1(std.term).decl.18: fn (TermRect, TermRect) -> bool
import import.19 __impl_131_intersection from module.1(std.term).decl.19: fn (TermRect, TermRect) -> TermRect
import import.20 __impl_131_clip from module.1(std.term).decl.20: fn (TermRect, TermRect) -> TermRectClip
import import.21 term_init from module.1(std.term).decl.39: fn () -> void
import import.22 term_done from module.1(std.term).decl.41: fn () -> void
import import.23 term_present from module.1(std.term).decl.68: fn () -> void
import import.24 term_loop from module.1(std.term).decl.69: fn () -> bool
import import.25 term_poll_event from module.1(std.term).decl.70: fn () -> TermEvent
import import.26 term_cls from module.1(std.term).decl.71: fn () -> void
import import.27 term_cursor_hide from module.1(std.term).decl.72: fn () -> void
import import.28 term_cursor_show from module.1(std.term).decl.73: fn () -> void
import import.29 term_cursor_goto from module.1(std.term).decl.74: fn (i32, i32) -> void
import import.30 term_rgb from module.1(std.term).decl.75: fn (u8, u8, u8) -> u32
import import.31 term_rgba from module.1(std.term).decl.76: fn (u8, u8, u8, u8) -> u32
import import.32 term_fb_cls from module.1(std.term).decl.102: fn (u32, u32) -> void
import import.33 term_fb_rect from module.1(std.term).decl.103: fn (TermRect, u32, u32, u32) -> void
import import.34 term_fb_write from module.1(std.term).decl.104: fn (i32, i32, string) -> void
import import.35 term_fb_write_cstr from module.1(std.term).decl.105: fn (i32, i32, ^u8^u8) -> v
import import.36 __impl_7445_init from module.1(std.term).decl.107: fn (^TermWindow^TermWindow, TermRe
import import.37 __impl_7445_done from module.1(std.term).decl.108: fn (^TermWindow^TermWind
import import.38 __impl_7445_clear from module.1(std.term).decl.109: fn (^TermWindow^TermWindow, u32, u32, u
import import.39 __impl_7445_paint_rect from module.1(std.term).decl.110: fn (^TermWindow^TermWindow, TermRect, u32, u32, u
import import.40 __impl_7445_draw_rect from module.1(std.term).decl.111: fn (^TermWindow^TermWindow, TermRect, u
import import.41 __impl_7445_paint from module.1(std.term).decl.112: fn (^TermWindow^TermWindow, TermRect, u32, u
import import.42 __impl_7445_nine_slice from module.1(std.term).decl.113: fn (^TermWindow^TermWindow, TermRect, ^u8^u8,
import import.43 __impl_7445_write from module.1(std.term).decl.114: fn (^TermWindow^TermWindow, i32, i32, stri
import import.44 __impl_7445_write_cstr from module.1(std.term).decl.115: fn (^TermWindow^TermWindow, i32, i32, ^
import import.45 __impl_7445_draw from module.1(std.term).decl.116: fn (^TermWindow^TermWind
import import.46 term_window_init from module.1(std.term).decl.117: fn (^TermWindow^TermWindow, TermRe
import import.47 term_window_done from module.1(std.term).decl.118: fn (^TermWindow^TermWind
import import.48 term_window_paint_rect from module.1(std.term).decl.119: fn (^TermWindow^TermWindow, TermRect, u32, u32, u
import import.49 term_window_clear from module.1(std.term).decl.120: fn (^TermWindow^TermWindow, u32, u32, u
import import.50 term_window_rect from module.1(std.term).decl.121: fn (^TermWindow^TermWindow, TermRect, u
import import.51 term_window_paint from module.1(std.term).decl.122: fn (^TermWindow^TermWindow, TermRect, u32, u
import import.52 term_window_9slice from module.1(std.term).decl.123: fn (^TermWindow^TermWindow, TermRect, ^u8^u8,
import import.53 term_window_write from module.1(std.term).decl.124: fn (^TermWindow^TermWindow, i32, i32, stri
import import.54 term_window_write_cstr from module.1(std.term).decl.125: fn (^TermWindow^TermWindow, i32, i32, ^
import import.55 term_window_draw from module.1(std.term).decl.126: fn (^TermWindow^TermWind
import import.56 __impl_126_init from module.1(std.term).decl.304: fn (^plex { ^u8^u8 base, usize cursor, usize committed_size, usize reserved_size, usize alloc_granularity, usize grow_rate }plex { ^u8^u8 base, 
import import.57 __impl_126_done from module.1(std.term).decl.305: fn (^plex { ^u8^u8 base, usize cursor, usize committed_size, usize reserved_size, usize alloc_granularity, usize grow_rate }plex {
import import.58 __impl_126_store from module.1(std.term).decl.306: fn (^plex { ^u8^u8 base, usize cursor, usize committed_size, usize reserved_size, usize alloc_granularity, usize grow_rate }plex { 
import import.59 __impl_126_restore from module.1(std.term).decl.307: fn (^plex { ^u8^u8 base, usize cursor, usize committed_size, usize reserved_size, usize alloc_granularity, usize grow_rate }plex { ^u8^u8
import import.60 __impl_126_reset from module.1(std.term).decl.308: fn (^plex { ^u8^u8 base, usize cursor, usize committed_size, usize reserved_size, usize alloc_granularity, usize grow_rate }plex {
bind Term = import.0
bind TermEvent = import.1
bind TermKeyEvent = import.2
bind TermMouseEvent = import.3
bind TermKeyCode = import.4
bind TERM_KEYMOD_CTRL = import.5
bind TERM_KEYMOD_ALT = import.6
bind TERM_KEYMOD_SHIFT = import.7
bind TermRect = import.8
bind TermRectClip = import.9
bind TermCell = import.10
bind TermWindow = import.11
bind term_size = import.12
bind __impl_131_to_point = import.13
bind __impl_131_union_with = import.14
bind __impl_131_is_empty = import.15
bind __impl_131_equals = import.16
bind __impl_131_contains = import.17
bind __impl_131_overlaps = import.18
bind __impl_131_intersection = import.19
bind __impl_131_clip = import.20
bind term_init = import.21
bind term_done = import.22
bind term_present = import.23
bind term_loop = import.24
bind term_poll_event = import.25
bind term_cls = import.26
bind term_cursor_hide = import.27
bind term_cursor_show = import.28
bind term_cursor_goto = import.29
bind term_rgb = import.30
bind term_rgba = import.31
bind term_fb_cls = import.32
bind term_fb_rect = import.33
bind term_fb_write = import.34
bind term_fb_write_cstr = import.35
bind __impl_7445_init = import.36
bind __impl_7445_done = import.37
bind __impl_7445_clear = import.38
bind __impl_7445_paint_rect = import.39
bind __impl_7445_draw_rect = import.40
bind __impl_7445_paint = import.41
bind __impl_7445_nine_slice = import.42
bind __impl_7445_write = import.43
bind __impl_7445_write_cstr = import.44
bind __impl_7445_draw = import.45
bind term_window_init = import.46
bind term_window_done = import.47
bind term_window_paint_rect = import.48
bind term_window_clear = import.49
bind term_window_rect = import.50
bind term_window_paint = import.51
bind term_window_9slice = import.52
bind term_window_write = import.53
bind term_window_write_cstr = import.54
bind term_window_draw = import.55
bind __impl_126_init = import.56
bind __impl_126_done = import.57
bind __impl_126_store = import.58
bind __impl_126_restore = import.59
bind __impl_126_reset = import.60
bind main = fn.0
bind Term = type.0
bind TermEvent = type.1
bind TermKeyEvent = type.2
bind TermMouseEvent = type.3
bind TermKeyCode = type.4
bind TERM_KEYMOD_CTRL = value.0
bind TERM_KEYMOD_ALT = value.1
bind TERM_KEYMOD_SHIFT = value.2
bind TermRect = type.5
bind TermRectClip = type.6
bind TermCell = type.7
bind TermWindow = type.8
type type.0 = Term
type type.1 = TermEvent
type type.2 = TermKeyEvent
type type.3 = TermMouseEvent
type type.4 = TermKeyCode
type type.5 = TermRect
type type.6 = TermRectClip
type type.7 = TermCell
type type.8 = TermWindow
const value.0: untyped integer
const value.1: untyped integer
const value.2: untyped integer
func fn.0() -> i32 {
  expr <unknown> <unsupported>
  let window: TermWindow = <unknown> <unsupported>
  let rect: TermRect = TermRect plex(x: u16 0, y: u16 0, width: u16 12, height: u16 4)
  let other: TermRect = TermRect plex(x: u16 2, y: u16 1, width: u16 4, height: u16 2)
  let overlap: TermRect = TermRect call bind.19(__impl_131_intersection)(TermRect local.1(rect), TermRect local.2(other))
  let reversed: TermRect = TermRect call bind.13(__impl_131_to_point)(TermRect plex(x: u16 6, y: u16 4, width: u16 0, height: u16 0), u16 3, u16 2)
  let combined: TermRect = TermRect call bind.14(__impl_131_union_with)(TermRect local.2(other), TermRect plex(x: u16 8, y: u16 3, width: u16 2, height: u16 2))
  let clip: TermRectClip = TermRectClip call bind.20(__impl_131_clip)(TermRect local.2(other), TermRect plex(x: u16 3, y: u16 0, width: u16 2, height: u16 3))
  expr void on bool call bind.15(__impl_131_is_empty)(TermRect local.1(rect)) {
    value(bool yes) => {
      return i32 1
    }
  }
  expr void on bool logical_or(bool not_equal(u16 field(TermRect local.3(overlap), x), u16 2), bool not_equal(u16 field(TermRect local.3(overlap), y), u16 1)) {
    value(bool yes) => {
      return i32 2
    }
  }
  expr void on bool logical_or(bool not_equal(u16 field(TermRect local.3(overlap), width), u16 4), bool not_equal(u16 field(TermRect local.3(overlap), height), u16 2)) {
    value(bool yes) => {
      return i32 3
    }
  }
  expr void on bool logical_not(bool call bind.16(__impl_131_equals)(TermRect local.3(overlap), TermRect local.2(other))) {
    value(bool yes) => {
      return i32 4
    }
  }
  expr void on bool logical_not(bool call bind.17(__impl_131_contains)(TermRect local.1(rect), u16 11, u16 3)) {
    value(bool yes) => {
      return i32 5
    }
  }
  expr void on bool call bind.17(__impl_131_contains)(TermRect local.1(rect), u16 12, u16 3) {
    value(bool yes) => {
      return i32 6
    }
  }
  expr void on bool logical_not(bool call bind.18(__impl_131_overlaps)(TermRect local.1(rect), TermRect local.2(other))) {
    value(bool yes) => {
      return i32 7
    }
  }
  expr void on bool call bind.18(__impl_131_overlaps)(TermRect local.2(other), TermRect plex(x: u16 20, y: u16 20, width: u16 2, height: u16 2)) {
    value(bool yes) => {
      return i32 8
    }
  }
  expr void on bool logical_or(bool not_equal(u16 field(TermRect local.4(reversed), x), u16 3), bool not_equal(u16 field(TermRect local.4(reversed), y), u16 2)) {
    value(bool yes) => {
      return i32 9
    }
  }
  expr void on bool logical_or(bool not_equal(u16 field(TermRect local.4(reversed), width), u16 4), bool not_equal(u16 field(TermRect local.4(reversed), height), u16 3)) {
    value(bool yes) => {
      return i32 10
    }
  }
  expr void on bool logical_or(bool not_equal(u16 field(TermRect local.5(combined), x), u16 2), bool not_equal(u16 field(TermRect local.5(combined), y), u16 1)) {
    value(bool yes) => {
      return i32 11
    }
  }
  expr void on bool logical_or(bool not_equal(u16 field(TermRect local.5(combined), width), u16 8), bool not_equal(u16 field(TermRect local.5(combined), height), u16 4)) {
    value(bool yes) => {
      return i32 12
    }
  }
  expr void on bool logical_not(bool field(TermRectClip local.6(clip), ok)) {
    value(bool yes) => {
      return i32 13
    }
  }
  expr void on bool logical_or(bool not_equal(u16 field(TermRect field(TermRectClip local.6(clip), clipped), x), u16 3), bool not_equal(u16 field(TermRect field(TermRectClip local.6(clip), clipped), y), u16 1)) {
    value(bool yes) => {
      return i32 14
    }
  }
  expr void on bool logical_or(bool not_equal(u16 field(TermRect field(TermRectClip local.6(clip), clipped), width), u16 2), bool not_equal(u16 field(TermRect field(TermRectClip local.6(clip), clipped), height), u16 2)) {
    value(bool yes) => {
      return i32 15
    }
  }
  expr void on bool logical_or(bool not_equal(u16 field(TermRect field(TermRectClip local.6(clip), local), x), u16 1), bool not_equal(u16 field(TermRect field(TermRectClip local.6(clip), local), y), u16 0)) {
    value(bool yes) => {
      return i32 16
    }
  }
  expr void call bind.46(term_window_init)(^TermWindow address_of(TermWindow local.0(window)), TermRect local.1(rect))
  let ink: u32 = u32 call bind.30(term_rgb)(u8 255, u8 255, u8 255)
  let paper: u32 = u32 call bind.30(term_rgb)(u8 0, u8 0, u8 0)
  expr void call bind.48(term_window_paint_rect)(^TermWindow address_of(TermWindow local.0(window)), TermRect local.1(rect), u32 cast(u8 32 as u32), u32 local.7(ink), u32 local.8(paper))
  expr void call bind.53(term_window_write)(^TermWindow address_of(TermWindow local.0(window)), i32 2, i32 1, string "ok")
  let first_cell: TermCell = TermCell index([..]TermCell field(TermWindow local.0(window), cells), usize cast(untyped integer add(untyped integer multiply(untyped integer 1, untyped integer 12), untyped integer 2) as usize))
  let second_cell: TermCell = TermCell index([..]TermCell field(TermWindow local.0(window), cells), usize cast(untyped integer add(untyped integer multiply(untyped integer 1, untyped integer 12), untyped integer 3) as usize))
  expr void call bind.47(term_window_done)(^TermWindow address_of(TermWindow local.0(window)))
  expr void on bool not_equal(u32 field(TermCell local.9(first_cell), ch), u32 cast(u8 111 as u32)) {
    value(bool yes) => {
      return i32 17
    }
  }
  expr void on bool not_equal(u32 field(TermCell local.10(second_cell), ch), u32 cast(u8 107 as u32)) {
    value(bool yes) => {
      return i32 18
    }
  }
  expr void on bool logical_or(bool not_equal(u32 field(TermCell local.9(first_cell), ink), u32 local.7(ink)), bool not_equal(u32 field(TermCell local.10(second_cell), ink), u32 local.7(ink))) {
    value(bool yes) => {
      return i32 19
    }
  }
  expr void on bool logical_or(bool not_equal(u32 field(TermCell local.9(first_cell), paper), u32 local.8(paper)), bool not_equal(u32 field(TermCell local.10(second_cell), paper), u32 local.8(paper))) {
    value(bool yes) => {
      return i32 20
    }
  }
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [3 x i8] c"ok\00"

declare ptr @malloc(i64)
declare ptr @realloc(ptr, i64)
declare void @free(ptr)

declare { i32, i32 } @$term_size()
declare { i16, i16, i16, i16 } @$__impl_131_to_point({ i16, i16, i16, i16 }, i16, i16)
declare { i16, i16, i16, i16 } @$__impl_131_union_with({ i16, i16, i16, i16 }, { i16, i16, i16, i16 })
declare i1 @$__impl_131_is_empty({ i16, i16, i16, i16 })
declare i1 @$__impl_131_equals({ i16, i16, i16, i16 }, { i16, i16, i16, i16 })
declare i1 @$__impl_131_contains({ i16, i16, i16, i16 }, i16, i16)
declare i1 @$__impl_131_overlaps({ i16, i16, i16, i16 }, { i16, i16, i16, i16 })
declare { i16, i16, i16, i16 } @$__impl_131_intersection({ i16, i16, i16, i16 }, { i16, i16, i16, i16 })
declare { i1, { i16, i16, i16, i16 }, { i16, i16, i16, i16 } } @$__impl_131_clip({ i16, i16, i16, i16 }, { i16, i16, i16, i16 })
declare void @$term_init()
declare void @$term_done()
declare void @$term_present()
declare i1 @$term_loop()
declare { i64, i192 } @$term_poll_event()
declare void @$term_cls()
declare void @$term_cursor_hide()
declare void @$term_cursor_show()
declare void @$term_cursor_goto(i32, i32)
declare i32 @$term_rgb(i8, i8, i8)
declare i32 @$term_rgba(i8, i8, i8, i8)
declare void @$term_fb_cls(i32, i32)
declare void @$term_fb_rect({ i16, i16, i16, i16 }, i32, i32, i32)
declare void @$term_fb_write(i32, i32, { ptr, i64 })
declare void @$term_fb_write_cstr(i32, i32, ptr)
declare void @$__impl_7445_init(ptr, { i16, i16, i16, i16 })
declare void @$__impl_7445_done(ptr)
declare void @$__impl_7445_clear(ptr, i32, i32, i32)
declare void @$__impl_7445_paint_rect(ptr, { i16, i16, i16, i16 }, i32, i32, i32)
declare void @$__impl_7445_draw_rect(ptr, { i16, i16, i16, i16 }, i32)
declare void @$__impl_7445_paint(ptr, { i16, i16, i16, i16 }, i32, i32)
declare void @$__impl_7445_nine_slice(ptr, { i16, i16, i16, i16 }, ptr, i1)
declare void @$__impl_7445_write(ptr, i32, i32, { ptr, i64 })
declare void @$__impl_7445_write_cstr(ptr, i32, i32, ptr)
declare void @$__impl_7445_draw(ptr)
declare void @$term_window_init(ptr, { i16, i16, i16, i16 })
declare void @$term_window_done(ptr)
declare void @$term_window_paint_rect(ptr, { i16, i16, i16, i16 }, i32, i32, i32)
declare void @$term_window_clear(ptr, i32, i32, i32)
declare void @$term_window_rect(ptr, { i16, i16, i16, i16 }, i32)
declare void @$term_window_paint(ptr, { i16, i16, i16, i16 }, i32, i32)
declare void @$term_window_9slice(ptr, { i16, i16, i16, i16 }, ptr, i1)
declare void @$term_window_write(ptr, i32, i32, { ptr, i64 })
declare void @$term_window_write_cstr(ptr, i32, i32, ptr)
declare void @$term_window_draw(ptr)
declare void @$__impl_126_init(ptr, i64, i64)
declare void @$__impl_126_done(ptr)
declare i64 @$__impl_126_store(ptr)
declare void @$__impl_126_restore(ptr, i64)
declare void @$__impl_126_reset(ptr)

define i32 @fn.0() {
  %t0 = insertvalue { i16, i16, i16, i16 } poison, i16 0, 0
  %t1 = insertvalue { i16, i16, i16, i16 } %t0, i16 0, 1
  %t2 = insertvalue { i16, i16, i16, i16 } %t1, i16 12, 2
  %t3 = insertvalue { i16, i16, i16, i16 } %t2, i16 4, 3
  %t4 = insertvalue { i16, i16, i16, i16 } poison, i16 2, 0
  %t5 = insertvalue { i16, i16, i16, i16 } %t4, i16 1, 1
  %t6 = insertvalue { i16, i16, i16, i16 } %t5, i16 4, 2
  %t7 = insertvalue { i16, i16, i16, i16 } %t6, i16 2, 3
  %t8 = call { i16, i16, i16, i16 } @$__impl_131_intersection({ i16, i16, i16, i16 } %t3, { i16, i16, i16, i16 } %t7)
  %t9 = insertvalue { i16, i16, i16, i16 } poison, i16 6, 0
  %t10 = insertvalue { i16, i16, i16, i16 } %t9, i16 4, 1
  %t11 = insertvalue { i16, i16, i16, i16 } %t10, i16 0, 2
  %t12 = insertvalue { i16, i16, i16, i16 } %t11, i16 0, 3
  %t13 = call { i16, i16, i16, i16 } @$__impl_131_to_point({ i16, i16, i16, i16 } %t12, i16 3, i16 2)
  %t14 = insertvalue { i16, i16, i16, i16 } poison, i16 8, 0
  %t15 = insertvalue { i16, i16, i16, i16 } %t14, i16 3, 1
  %t16 = insertvalue { i16, i16, i16, i16 } %t15, i16 2, 2
  %t17 = insertvalue { i16, i16, i16, i16 } %t16, i16 2, 3
  %t18 = call { i16, i16, i16, i16 } @$__impl_131_union_with({ i16, i16, i16, i16 } %t7, { i16, i16, i16, i16 } %t17)
  %t19 = insertvalue { i16, i16, i16, i16 } poison, i16 3, 0
  %t20 = insertvalue { i16, i16, i16, i16 } %t19, i16 0, 1
  %t21 = insertvalue { i16, i16, i16, i16 } %t20, i16 2, 2
  %t22 = insertvalue { i16, i16, i16, i16 } %t21, i16 3, 3
  %t23 = call { i1, { i16, i16, i16, i16 }, { i16, i16, i16, i16 } } @$__impl_131_clip({ i16, i16, i16, i16 } %t7, { i16, i16, i16, i16 } %t22)
  %t24 = call i1 @$__impl_131_is_empty({ i16, i16, i16, i16 } %t3)
  %t25 = icmp eq i1 %t24, 1
  br i1 %t25, label %on.body.1, label %on.end.0
on.body.1:
  ret i32 1
on.end.0:
  %t26 = extractvalue { i16, i16, i16, i16 } %t8, 0
  %t27 = icmp ne i16 %t26, 2
  %t28 = extractvalue { i16, i16, i16, i16 } %t8, 1
  %t29 = icmp ne i16 %t28, 1
  %t30 = or i1 %t27, %t29
  %t31 = icmp eq i1 %t30, 1
  br i1 %t31, label %on.body.3, label %on.end.2
on.body.3:
  ret i32 2
on.end.2:
  %t32 = extractvalue { i16, i16, i16, i16 } %t8, 2
  %t33 = icmp ne i16 %t32, 4
  %t34 = extractvalue { i16, i16, i16, i16 } %t8, 3
  %t35 = icmp ne i16 %t34, 2
  %t36 = or i1 %t33, %t35
  %t37 = icmp eq i1 %t36, 1
  br i1 %t37, label %on.body.5, label %on.end.4
on.body.5:
  ret i32 3
on.end.4:
  %t38 = call i1 @$__impl_131_equals({ i16, i16, i16, i16 } %t8, { i16, i16, i16, i16 } %t7)
  %t39 = xor i1 %t38, 1
  %t40 = icmp eq i1 %t39, 1
  br i1 %t40, label %on.body.7, label %on.end.6
on.body.7:
  ret i32 4
on.end.6:
  %t41 = call i1 @$__impl_131_contains({ i16, i16, i16, i16 } %t3, i16 11, i16 3)
  %t42 = xor i1 %t41, 1
  %t43 = icmp eq i1 %t42, 1
  br i1 %t43, label %on.body.9, label %on.end.8
on.body.9:
  ret i32 5
on.end.8:
  %t44 = call i1 @$__impl_131_contains({ i16, i16, i16, i16 } %t3, i16 12, i16 3)
  %t45 = icmp eq i1 %t44, 1
  br i1 %t45, label %on.body.11, label %on.end.10
on.body.11:
  ret i32 6
on.end.10:
  %t46 = call i1 @$__impl_131_overlaps({ i16, i16, i16, i16 } %t3, { i16, i16, i16, i16 } %t7)
  %t47 = xor i1 %t46, 1
  %t48 = icmp eq i1 %t47, 1
  br i1 %t48, label %on.body.13, label %on.end.12
on.body.13:
  ret i32 7
on.end.12:
  %t49 = insertvalue { i16, i16, i16, i16 } poison, i16 20, 0
  %t50 = insertvalue { i16, i16, i16, i16 } %t49, i16 20, 1
  %t51 = insertvalue { i16, i16, i16, i16 } %t50, i16 2, 2
  %t52 = insertvalue { i16, i16, i16, i16 } %t51, i16 2, 3
  %t53 = call i1 @$__impl_131_overlaps({ i16, i16, i16, i16 } %t7, { i16, i16, i16, i16 } %t52)
  %t54 = icmp eq i1 %t53, 1
  br i1 %t54, label %on.body.15, label %on.end.14
on.body.15:
  ret i32 8
on.end.14:
  %t55 = extractvalue { i16, i16, i16, i16 } %t13, 0
  %t56 = icmp ne i16 %t55, 3
  %t57 = extractvalue { i16, i16, i16, i16 } %t13, 1
  %t58 = icmp ne i16 %t57, 2
  %t59 = or i1 %t56, %t58
  %t60 = icmp eq i1 %t59, 1
  br i1 %t60, label %on.body.17, label %on.end.16
on.body.17:
  ret i32 9
on.end.16:
  %t61 = extractvalue { i16, i16, i16, i16 } %t13, 2
  %t62 = icmp ne i16 %t61, 4
  %t63 = extractvalue { i16, i16, i16, i16 } %t13, 3
  %t64 = icmp ne i16 %t63, 3
  %t65 = or i1 %t62, %t64
  %t66 = icmp eq i1 %t65, 1
  br i1 %t66, label %on.body.19, label %on.end.18
on.body.19:
  ret i32 10
on.end.18:
  %t67 = extractvalue { i16, i16, i16, i16 } %t18, 0
  %t68 = icmp ne i16 %t67, 2
  %t69 = extractvalue { i16, i16, i16, i16 } %t18, 1
  %t70 = icmp ne i16 %t69, 1
  %t71 = or i1 %t68, %t70
  %t72 = icmp eq i1 %t71, 1
  br i1 %t72, label %on.body.21, label %on.end.20
on.body.21:
  ret i32 11
on.end.20:
  %t73 = extractvalue { i16, i16, i16, i16 } %t18, 2
  %t74 = icmp ne i16 %t73, 8
  %t75 = extractvalue { i16, i16, i16, i16 } %t18, 3
  %t76 = icmp ne i16 %t75, 4
  %t77 = or i1 %t74, %t76
  %t78 = icmp eq i1 %t77, 1
  br i1 %t78, label %on.body.23, label %on.end.22
on.body.23:
  ret i32 12
on.end.22:
  %t79 = extractvalue { i1, { i16, i16, i16, i16 }, { i16, i16, i16, i16 } } %t23, 0
  %t80 = xor i1 %t79, 1
  %t81 = icmp eq i1 %t80, 1
  br i1 %t81, label %on.body.25, label %on.end.24
on.body.25:
  ret i32 13
on.end.24:
  %t82 = extractvalue { i1, { i16, i16, i16, i16 }, { i16, i16, i16, i16 } } %t23, 1
  %t83 = extractvalue { i16, i16, i16, i16 } %t82, 0
  %t84 = icmp ne i16 %t83, 3
  %t85 = extractvalue { i1, { i16, i16, i16, i16 }, { i16, i16, i16, i16 } } %t23, 1
  %t86 = extractvalue { i16, i16, i16, i16 } %t85, 1
  %t87 = icmp ne i16 %t86, 1
  %t88 = or i1 %t84, %t87
  %t89 = icmp eq i1 %t88, 1
  br i1 %t89, label %on.body.27, label %on.end.26
on.body.27:
  ret i32 14
on.end.26:
  %t90 = extractvalue { i1, { i16, i16, i16, i16 }, { i16, i16, i16, i16 } } %t23, 1
  %t91 = extractvalue { i16, i16, i16, i16 } %t90, 2
  %t92 = icmp ne i16 %t91, 2
  %t93 = extractvalue { i1, { i16, i16, i16, i16 }, { i16, i16, i16, i16 } } %t23, 1
  %t94 = extractvalue { i16, i16, i16, i16 } %t93, 3
  %t95 = icmp ne i16 %t94, 2
  %t96 = or i1 %t92, %t95
  %t97 = icmp eq i1 %t96, 1
  br i1 %t97, label %on.body.29, label %on.end.28
on.body.29:
  ret i32 15
on.end.28:
  %t98 = extractvalue { i1, { i16, i16, i16, i16 }, { i16, i16, i16, i16 } } %t23, 2
  %t99 = extractvalue { i16, i16, i16, i16 } %t98, 0
  %t100 = icmp ne i16 %t99, 1
  %t101 = extractvalue { i1, { i16, i16, i16, i16 }, { i16, i16, i16, i16 } } %t23, 2
  %t102 = extractvalue { i16, i16, i16, i16 } %t101, 1
  %t103 = icmp ne i16 %t102, 0
  %t104 = or i1 %t100, %t103
  %t105 = icmp eq i1 %t104, 1
  br i1 %t105, label %on.body.31, label %on.end.30
on.body.31:
  ret i32 16
on.end.30:
  %local.0 = alloca { { i16, i16, i16, i16 }, ptr }
  store { { i16, i16, i16, i16 }, ptr } zeroinitializer, ptr %local.0
  call void @$term_window_init(ptr %local.0, { i16, i16, i16, i16 } %t3)
  %t106 = call i32 @$term_rgb(i8 255, i8 255, i8 255)
  %t107 = call i32 @$term_rgb(i8 0, i8 0, i8 0)
  %t108 = zext i8 32 to i32
  call void @$term_window_paint_rect(ptr %local.0, { i16, i16, i16, i16 } %t3, i32 %t108, i32 %t106, i32 %t107)
  call void @$term_window_write(ptr %local.0, i32 2, i32 1, { ptr, i64 } { ptr @.str.m0.0, i64 2 })
  %t109 = load { { i16, i16, i16, i16 }, ptr }, ptr %local.0
  %t110 = extractvalue { { i16, i16, i16, i16 }, ptr } %t109, 1
  %t111 = mul i32 1, 12
  %t112 = add i32 %t111, 2
  %t113 = sext i32 %t112 to i64
  %t114 = getelementptr inbounds { ptr, i64, i64 }, ptr %t110, i64 0, i32 0
  %t115 = load ptr, ptr %t114
  %t116 = getelementptr inbounds { i32, i32, i32 }, ptr %t115, i64 %t113
  %t117 = load { i32, i32, i32 }, ptr %t116
  %t118 = load { { i16, i16, i16, i16 }, ptr }, ptr %local.0
  %t119 = extractvalue { { i16, i16, i16, i16 }, ptr } %t118, 1
  %t120 = mul i32 1, 12
  %t121 = add i32 %t120, 3
  %t122 = sext i32 %t121 to i64
  %t123 = getelementptr inbounds { ptr, i64, i64 }, ptr %t119, i64 0, i32 0
  %t124 = load ptr, ptr %t123
  %t125 = getelementptr inbounds { i32, i32, i32 }, ptr %t124, i64 %t122
  %t126 = load { i32, i32, i32 }, ptr %t125
  call void @$term_window_done(ptr %local.0)
  %t127 = extractvalue { i32, i32, i32 } %t117, 2
  %t128 = zext i8 111 to i32
  %t129 = icmp ne i32 %t127, %t128
  %t130 = icmp eq i1 %t129, 1
  br i1 %t130, label %on.body.33, label %on.end.32
on.body.33:
  ret i32 17
on.end.32:
  %t131 = extractvalue { i32, i32, i32 } %t126, 2
  %t132 = zext i8 107 to i32
  %t133 = icmp ne i32 %t131, %t132
  %t134 = icmp eq i1 %t133, 1
  br i1 %t134, label %on.body.35, label %on.end.34
on.body.35:
  ret i32 18
on.end.34:
  %t135 = extractvalue { i32, i32, i32 } %t117, 0
  %t136 = icmp ne i32 %t135, %t106
  %t137 = extractvalue { i32, i32, i32 } %t126, 0
  %t138 = icmp ne i32 %t137, %t106
  %t139 = or i1 %t136, %t138
  %t140 = icmp eq i1 %t139, 1
  br i1 %t140, label %on.body.37, label %on.end.36
on.body.37:
  ret i32 19
on.end.36:
  %t141 = extractvalue { i32, i32, i32 } %t117, 1
  %t142 = icmp ne i32 %t141, %t107
  %t143 = extractvalue { i32, i32, i32 } %t126, 1
  %t144 = icmp ne i32 %t143, %t107
  %t145 = or i1 %t142, %t144
  %t146 = icmp eq i1 %t145, 1
  br i1 %t146, label %on.body.39, label %on.end.38
on.body.39:
  ret i32 20
on.end.38:
  ret i32 0
}

@$main = alias i32 (), ptr @fn.0
