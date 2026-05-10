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

@$left = global { i32, ptr } zeroinitializer
@$right = global { i32, ptr } zeroinitializer

define void @m0.init() {
  %t0 = insertvalue { i32, ptr } poison, i32 1, 0
  %t1 = insertvalue { i32, ptr } %t0, ptr null, 1
  store { i32, ptr } %t1, ptr @$left
  %t2 = load { i32, ptr }, ptr @$left
  %t3 = alloca { i32, ptr }
  store { i32, ptr } %t2, ptr %t3
  %t4 = insertvalue { i32, ptr } poison, i32 2, 0
  %t5 = insertvalue { i32, ptr } %t4, ptr %t3, 1
  store { i32, ptr } %t5, ptr @$right
  ret void
}

define i32 @fn.0() {
  %t0 = load { i32, ptr }, ptr @$right
  %t1 = alloca { i32, ptr }
  store { i32, ptr } %t0, ptr %t1
  %t2 = load { i32, ptr }, ptr @$left
  %t3 = alloca { i32, ptr }
  store { i32, ptr } %t2, ptr %t3
  %t4 = getelementptr inbounds { i32, ptr }, ptr %t3, i64 0, i32 1
  store ptr %t1, ptr %t4
  %t5 = load { i32, ptr }, ptr @$left
  %t6 = extractvalue { i32, ptr } %t5, 1
  %t7 = load { i32, ptr }, ptr %t6
  %t8 = extractvalue { i32, ptr } %t7, 0
  %t9 = load { i32, ptr }, ptr @$right
  %t10 = extractvalue { i32, ptr } %t9, 1
  %t11 = load { i32, ptr }, ptr %t10
  %t12 = extractvalue { i32, ptr } %t11, 0
  %t13 = add i32 %t8, %t12
  ret i32 %t13
}

@$main = alias i32 (), ptr @fn.0
