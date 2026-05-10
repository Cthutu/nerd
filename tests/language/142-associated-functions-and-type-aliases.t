Box :: plex [T] {
    value T
}

IntBox :: Box[i32]

global_box := IntBox.init(40)

impl Box[T] {
    init :: fn (value: T) -> Self {
        return { value }
    }

    item_size :: fn (self: Self) -> usize {
        return T.size
    }
}

main :: fn () -> i32 {
    box := IntBox.init(2)
    return global_box.value + box.value + box.item_size().as(i32) - i32.size.as(i32)
}
¬
42
¬

¬
hir 0
bind Box = type.0
bind IntBox = type.1
bind global_box = value.0
bind main = fn.0
generic type type.0 = <unknown>
type type.1 = IntBox
global value.0: IntBox = IntBox call fn (i32) -> IntBox field(IntBox bind.1(IntBox), init)(i32 40)
func fn.0() -> i32 {
  let box: IntBox = IntBox call fn (i32) -> IntBox field(IntBox bind.1(IntBox), init)(i32 2)
  return i32 subtract(i32 add(i32 add(i32 field(IntBox bind.2(global_box), value), i32 field(IntBox local.0(box), value)), i32 cast(usize call decl.4(__impl_16_item_size_g_549f6c0d)(IntBox local.0(box)) as i32)), i32 cast(usize field(i32 i32, size) as i32))
}
inst func fn.1(value: i32) -> IntBox {
  return IntBox plex(value: i32 local.1(value))
}
inst func fn.2(self: IntBox) -> usize {
  return usize field(i32 T, size)
}
¬
; nerd llvm-ir 0
; generated from HIR

@$global_box = global { i32 } zeroinitializer

define void @m0.init() {
  %t0 = call { i32 } @$init(i32 40)
  store { i32 } %t0, ptr @$global_box
  ret void
}

define i32 @fn.0() {
  %t0 = call { i32 } @$init(i32 2)
  %t1 = load { i32 }, ptr @$global_box
  %t2 = extractvalue { i32 } %t1, 0
  %t3 = extractvalue { i32 } %t0, 0
  %t4 = add i32 %t2, %t3
  %t5 = trunc i64 4 to i32
  ret i32 0
}

define { i32 } @fn.1(i32 %value) {
  %t0 = insertvalue { i32 } poison, i32 %value, 0
  ret { i32 } %t0
}

define i64 @fn.2({ i32 } %self) {
  ret i64 4
}

@$main = alias i32 (), ptr @fn.0
