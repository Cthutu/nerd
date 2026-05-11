-- test-platform: linux

arena :: use std.arena

main :: fn () -> i32 {
    a := arena.arena_new(16)
    result := arena.arena_capacity(^a).as(i32) - 16
    block := arena.arena_alloc(^a, 4)
    result += block.count.as(i32) - 4
    arena.arena_free(^a)
    return result
}
¬
0
¬

¬
hir 0
module module.0(092-stdlib-arena.input)
import module.1(std.arena)
import import.0 __impl_126_init from module.1(std.arena).decl.7: fn (^plex { ^u8^u8 base, usize cursor, usize committed_size, usize reserved_size, usize alloc_granularity, usize grow_rate }plex { ^u8^u8 base, 
import import.1 __impl_126_done from module.1(std.arena).decl.8: fn (^plex { ^u8^u8 base, usize cursor, usize committed_size, usize reserved_size, usize alloc_granularity, usize grow_rate }plex {
import import.2 __impl_126_store from module.1(std.arena).decl.9: fn (^plex { ^u8^u8 base, usize cursor, usize committed_size, usize reserved_size, usize alloc_granularity, usize grow_rate }plex { 
import import.3 __impl_126_restore from module.1(std.arena).decl.10: fn (^plex { ^u8^u8 base, usize cursor, usize committed_size, usize reserved_size, usize alloc_granularity, usize grow_rate }plex { ^u8^u8
import import.4 __impl_126_reset from module.1(std.arena).decl.11: fn (^plex { ^u8^u8 base, usize cursor, usize committed_size, usize reserved_size, usize alloc_granularity, usize grow_rate }plex {
import import.5 arena_new from module.1(std.arena).decl.2: fn (usize) -> plex { []u8[]u8 base, usize curs
import import.6 arena_capacity from module.1(std.arena).decl.3: fn (^plex { []u8[]u8 base, usize cursor }plex {
import import.7 arena_alloc from module.1(std.arena).decl.4: fn (^plex { []u8[]u8 base, usize cursor }plex { []u8[
import import.8 arena_free from module.1(std.arena).decl.5: fn (^plex { []u8[]u8 base, usize cursor }plex 
bind __impl_126_init = import.0
bind __impl_126_done = import.1
bind __impl_126_store = import.2
bind __impl_126_restore = import.3
bind __impl_126_reset = import.4
bind arena_new = import.5
bind arena_capacity = import.6
bind arena_alloc = import.7
bind arena_free = import.8
bind arena = module.1
bind main = fn.0
func fn.0() -> i32 {
  let a: plex { []u8[]u8 base, usize curs = plex { []u8[]u8 base, usize curs call fn (usize) -> plex { []u8[]u8 base, usize curs field(module bind.9(arena), arena_new)(usize 16)
  let result: i32 = i32 subtract(i32 cast(usize call fn (^plex { []u8[]u8 base, usize cursor }plex { field(module bind.9(arena), arena_capacity)(^plex { []u8[]u8 base, usize curs address_of(plex { []u8[]u8 base, usize curs local.0(a))) as i32), i32 16)
  let block: []u8 = []u8 call fn (^plex { []u8[]u8 base, usize cursor }plex { []u8[ field(module bind.9(arena), arena_alloc)(^plex { []u8[]u8 base, usize curs address_of(plex { []u8[]u8 base, usize curs local.0(a)), usize 4)
  assign i32 local.1(result) = i32 add(i32 local.1(result), i32 subtract(i32 cast(usize field([]u8 local.2(block), count) as i32), i32 4))
  expr void call fn (^plex { []u8[]u8 base, usize cursor }plex  field(module bind.9(arena), arena_free)(^plex { []u8[]u8 base, usize curs address_of(plex { []u8[]u8 base, usize curs local.0(a)))
  return i32 local.1(result)
}
¬
; nerd llvm-ir 0
; generated from HIR

declare void @$__impl_126_init(ptr, i64, i64)
declare void @$__impl_126_done(ptr)
declare i64 @$__impl_126_store(ptr)
declare void @$__impl_126_restore(ptr, i64)
declare void @$__impl_126_reset(ptr)
declare { { ptr, i64 }, i64 } @$arena_new(i64)
declare i64 @$arena_capacity(ptr)
declare { ptr, i64 } @$arena_alloc(ptr, i64)
declare void @$arena_free(ptr)

define internal i32 @fn.0() {
  %t0 = call { { ptr, i64 }, i64 } @$arena_new(i64 16)
  %local.0 = alloca { { ptr, i64 }, i64 }
  store { { ptr, i64 }, i64 } %t0, ptr %local.0
  %t1 = call i64 @$arena_capacity(ptr %local.0)
  %t2 = trunc i64 %t1 to i32
  %t3 = sub i32 %t2, 16
  %local.1 = alloca i32
  store i32 %t3, ptr %local.1
  %t4 = call { ptr, i64 } @$arena_alloc(ptr %local.0, i64 4)
  %t5 = load i32, ptr %local.1
  %t6 = extractvalue { ptr, i64 } %t4, 1
  %t7 = trunc i64 %t6 to i32
  %t8 = sub i32 %t7, 4
  %t9 = add i32 %t5, %t8
  store i32 %t9, ptr %local.1
  call void @$arena_free(ptr %local.0)
  %t10 = load i32, ptr %local.1
  ret i32 %t10
}

@$main = alias i32 (), ptr @fn.0
