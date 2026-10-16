Left :: plex {
    value i32
    right ^Right
}

Right :: plex {
    value i32
    left  ^Left
}

left: Left = { value: 1, ... }
right: Right = { value: 2, left: ^left }

main :: fn () -> i32 {
    left.right = ^right
    return left.right.value + right.left.value
}
¬
3
¬

¬
hir 0
bind Left = type.0
bind Right = type.1
bind left = value.0
bind right = value.1
bind main = fn.0
type type.0 = Left
type type.1 = Right
global value.0: Left = Left plex(value: i32 1, ...)
global value.1: Right = Right plex(value: i32 2, left: ^Left address_of(Left bind.2(left)))
func fn.0() -> i32 {
  assign ^Right field(Left bind.2(left), right) = ^Right address_of(Right bind.3(right))
  return i32 add(i32 field(^Right field(Left bind.2(left), right), value), i32 field(^Left field(Right bind.3(right), left), value))
}
¬
; nerd llvm-ir 0
; generated from HIR

@$left = internal global { ptr, i32 } zeroinitializer
@$right = internal global { ptr, i32 } zeroinitializer

define void @m0.init() {
  %t0 = insertvalue { ptr, i32 } poison, i32 1, 1
  %t1 = insertvalue { ptr, i32 } %t0, ptr null, 0
  store { ptr, i32 } %t1, ptr @$left
  %t2 = insertvalue { ptr, i32 } poison, i32 2, 1
  %t3 = insertvalue { ptr, i32 } %t2, ptr @$left, 0
  store { ptr, i32 } %t3, ptr @$right
  ret void
}

define internal i32 @fn.0() {
  %t0 = getelementptr inbounds { ptr, i32 }, ptr @$left, i64 0, i32 0
  store ptr @$right, ptr %t0
  %t1 = load { ptr, i32 }, ptr @$left
  %t2 = extractvalue { ptr, i32 } %t1, 0
  %t3 = load { ptr, i32 }, ptr %t2
  %t4 = extractvalue { ptr, i32 } %t3, 1
  %t5 = load { ptr, i32 }, ptr @$right
  %t6 = extractvalue { ptr, i32 } %t5, 0
  %t7 = load { ptr, i32 }, ptr %t6
  %t8 = extractvalue { ptr, i32 } %t7, 1
  %t9 = add i32 %t4, %t8
  ret i32 %t9
}

@$main = hidden alias i32 (), ptr @fn.0
