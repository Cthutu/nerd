use std.io

Rect :: plex {
    width  i32
    height i32
}

impl Rect {

    area :: fn (self: Self) -> i32 {
        return self.width * self.height
    }

    grow :: fn (self: ^Self, amount: i32) {
        self.width += amount
        self.height += amount
    }

}

use_methods :: fn (rect: ^Rect) -> i32 {
    rect.grow(2)
    return rect.area()
}

main :: fn () -> i32 {
    rect: Rect
    rect.grow(3)
    result := use_methods(^rect)
    prn($"area {result}")
    return 0
}
¬
0
¬
area 25

¬
hir 0
module module.0(141-method-regressions.input)
import module.1(std.io)
import import.0 pr from module.1(std.io).decl.9: fn (string) -> void
import import.1 epr from module.1(std.io).decl.10: fn (string) -> void
import import.2 prn from module.1(std.io).decl.11: fn (string) -> void
import import.3 eprn from module.1(std.io).decl.12: fn (string) -> void
import import.4 input from module.1(std.io).decl.13: fn (string) -> string
bind pr = import.0
bind epr = import.1
bind prn = import.2
bind eprn = import.3
bind input = import.4
bind Rect = type.0
bind __impl_7_area = fn.0
bind __impl_7_grow = fn.1
bind use_methods = fn.2
bind main = fn.3
type type.0 = Rect
func fn.0(self: Rect) -> i32 {
  return i32 multiply(i32 field(Rect local.0(self), width), i32 field(Rect local.0(self), height))
}
func fn.1(self: ^Rect, amount: i32) -> void {
  assign i32 field(^Rect local.1(self), width) = i32 add(i32 field(^Rect local.1(self), width), i32 local.2(amount))
  assign i32 field(^Rect local.1(self), height) = i32 add(i32 field(^Rect local.1(self), height), i32 local.2(amount))
}
func fn.2(rect: ^Rect) -> i32 {
  expr void call bind.7(__impl_7_grow)(^Rect local.3(rect), i32 2)
  return i32 call bind.6(__impl_7_area)(Rect deref(^Rect local.3(rect)))
}
func fn.3() -> i32 {
  expr <unknown> <unsupported>
  let rect: Rect = <unknown> <unsupported>
  expr void call bind.7(__impl_7_grow)(^Rect address_of(Rect local.4(rect)), i32 3)
  let result: i32 = i32 call bind.8(use_methods)(^Rect address_of(Rect local.4(rect)))
  expr void call bind.2(prn)(string interpolate(<unknown> "area ", i32 local.5(result)))
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [6 x i8] c"area \00"

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

declare void @$pr({ ptr, i64 })
declare void @$epr({ ptr, i64 })
declare void @$prn({ ptr, i64 })
declare void @$eprn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

define i32 @fn.0({ i32, i32 } %self) {
  %t0 = extractvalue { i32, i32 } %self, 0
  %t1 = extractvalue { i32, i32 } %self, 1
  %t2 = mul i32 %t0, %t1
  ret i32 %t2
}

define void @fn.1(ptr %self, i32 %amount) {
  %t0 = load { i32, i32 }, ptr %self
  %t1 = extractvalue { i32, i32 } %t0, 0
  %t2 = add i32 %t1, %amount
  %t3 = getelementptr inbounds { i32, i32 }, ptr %self, i64 0, i32 0
  store i32 %t2, ptr %t3
  %t4 = load { i32, i32 }, ptr %self
  %t5 = extractvalue { i32, i32 } %t4, 1
  %t6 = add i32 %t5, %amount
  %t7 = getelementptr inbounds { i32, i32 }, ptr %self, i64 0, i32 1
  store i32 %t6, ptr %t7
  ret void
}

define i32 @fn.2(ptr %rect) {
  call void @fn.1(ptr %rect, i32 2)
  %t0 = load { i32, i32 }, ptr %rect
  %t1 = call i32 @fn.0({ i32, i32 } %t0)
  ret i32 %t1
}

define i32 @fn.3() {
  %local.4 = alloca { i32, i32 }
  store { i32, i32 } zeroinitializer, ptr %local.4
  call void @fn.1(ptr %local.4, i32 3)
  %t0 = call i32 @fn.2(ptr %local.4)
  %t1 = call i64 @string_builder_mark()
  %t2 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 5 })
  call void @string_builder_append_string({ ptr, i64 } %t2)
  %t3 = call { ptr, i64 } @to_string$i32(i32 %t0)
  call void @string_builder_append_string({ ptr, i64 } %t3)
  %t4 = call { ptr, i64 } @string_builder_finish(i64 %t1)
  call void @$prn({ ptr, i64 } %t4)
  ret i32 0
}

@$__impl_7_area = internal alias i32 ({ i32, i32 }), ptr @fn.0
@$__impl_7_grow = internal alias void (ptr, i32), ptr @fn.1
@$use_methods = internal alias i32 (ptr), ptr @fn.2
@$main = alias i32 (), ptr @fn.3
