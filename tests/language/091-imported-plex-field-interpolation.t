boxmod :: use test.imported_plex

consume :: fn (text: string) {
    on text.count == 0 => return
    return
}

main :: fn () {
    box := boxmod.make_box(7)
    consume($"Value: {box.value}")
}
¬
0
¬

¬
hir 0
module module.0(091-imported-plex-field-interpolation.input)
import module.1(test.imported_plex)
import import.0 __impl_4_bump from module.1(test.imported_plex).decl.1: fn (^plex { usize value }plex { usize val
import import.1 make_box from module.1(test.imported_plex).decl.2: fn (usize) -> plex { usize value }
bind __impl_4_bump = import.0
bind make_box = import.1
bind boxmod = module.1
bind consume = fn.0
bind main = fn.1
func fn.0(text: string) -> void {
  expr void on bool equal(usize field(string local.0(text), count), usize 0) {
    value(bool yes) => {
      return <none>
    }
  }
  return <none>
}
func fn.1() -> void {
  let box: plex { usize value } = plex { usize value } call fn (usize) -> plex { usize value } field(module bind.2(boxmod), make_box)(usize 7)
  expr void call bind.3(consume)(string interpolate(<unknown> "Value: ", usize field(plex { usize value } local.1(box), value)))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [8 x i8] c"Value: \00"

declare i1 @string_eq({ ptr, i64 }, { ptr, i64 })
declare void @string_builder_reset()
declare i64 @string_builder_mark()
declare void @string_builder_append_string({ ptr, i64 })
declare void @string_builder_append_byte(i8)
declare { ptr, i64 } @string_builder_finish(i64)
declare { ptr, i64 } @to_string$string({ ptr, i64 })
declare { ptr, i64 } @to_string$bool(i1)
declare { ptr, i64 } @to_string$i8(i8)
declare { ptr, i64 } @to_string$i16(i16)
declare { ptr, i64 } @to_string$i32(i32)
declare { ptr, i64 } @to_string$i64(i64)
declare { ptr, i64 } @to_string$u8(i8)
declare { ptr, i64 } @to_string$u16(i16)
declare { ptr, i64 } @to_string$u32(i32)
declare { ptr, i64 } @to_string$u64(i64)
declare { ptr, i64 } @to_string$isize(i64)
declare { ptr, i64 } @to_string$usize(i64)
declare { ptr, i64 } @to_string$f32(float)
declare { ptr, i64 } @to_string$f64(double)

declare void @$__impl_4_bump(ptr, i64)
declare { i64 } @$make_box(i64)

define void @fn.0({ ptr, i64 } %text) {
  %t0 = extractvalue { ptr, i64 } %text, 1
  %t1 = icmp eq i64 %t0, 0
  %t2 = icmp eq i1 %t1, 1
  br i1 %t2, label %on.body.1, label %on.end.0
on.body.1:
  ret void
on.end.0:
  ret void
}

define void @fn.1() {
  %t0 = call { i64 } @$make_box(i64 7)
  %t1 = call i64 @string_builder_mark()
  %t2 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 7 })
  call void @string_builder_append_string({ ptr, i64 } %t2)
  %t3 = extractvalue { i64 } %t0, 0
  %t4 = call { ptr, i64 } @to_string$usize(i64 %t3)
  call void @string_builder_append_string({ ptr, i64 } %t4)
  %t5 = call { ptr, i64 } @string_builder_finish(i64 %t1)
  call void @fn.0({ ptr, i64 } %t5)
  ret void
}

@$consume = alias void ({ ptr, i64 }), ptr @fn.0
@$main = alias void (), ptr @fn.1
