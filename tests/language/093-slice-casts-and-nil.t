main :: fn () -> i32 {
    text: [4]u8 = ['a', 'b', 'c', 'd']
    ptr := text[..].data
    view := ptr.as([]u8, 2)
    empty: []u8 = nil

    return view.count.as(i32) * 100 + view[0].as(i32) + empty.count.as(i32)
}
¬
41
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  let text: [4]u8 = [4]u8 array(u8 97, u8 98, u8 99, u8 100)
  let ptr: ^u8 = ^u8 field([]u8 slice([4]u8 local.0(text), <none>, <none>), data)
  let view: []u8 = []u8 cast(^u8 local.1(ptr) as []u8, usize 2)
  let empty: []u8 = []u8 nil
  return i32 add(i32 add(i32 multiply(i32 cast(usize field([]u8 local.2(view), count) as i32), i32 100), i32 cast(u8 index([]u8 local.2(view), untyped integer 0) as i32)), i32 cast(usize field([]u8 local.3(empty), count) as i32))
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0() {
  %t0 = insertvalue [4 x i8] poison, i8 97, 0
  %t1 = insertvalue [4 x i8] %t0, i8 98, 1
  %t2 = insertvalue [4 x i8] %t1, i8 99, 2
  %t3 = insertvalue [4 x i8] %t2, i8 100, 3
  %local.0 = alloca [4 x i8]
  store [4 x i8] %t3, ptr %local.0
  %t4 = getelementptr inbounds [4 x i8], ptr %local.0, i64 0, i64 0
  %t5 = insertvalue { ptr, i64 } poison, ptr %t4, 0
  %t6 = insertvalue { ptr, i64 } %t5, i64 4, 1
  %t7 = extractvalue { ptr, i64 } %t6, 0
  %t8 = insertvalue { ptr, i64 } poison, ptr %t7, 0
  %t9 = insertvalue { ptr, i64 } %t8, i64 2, 1
  %t10 = extractvalue { ptr, i64 } %t9, 1
  %t11 = trunc i64 %t10 to i32
  %t12 = mul i32 %t11, 100
  %t13 = extractvalue { ptr, i64 } %t9, 0
  %t14 = getelementptr inbounds i8, ptr %t13, i32 0
  %t15 = load i8, ptr %t14
  %t16 = zext i8 %t15 to i32
  %t17 = add i32 %t12, %t16
  %t18 = extractvalue { ptr, i64 } zeroinitializer, 1
  %t19 = trunc i64 %t18 to i32
  %t20 = add i32 %t17, %t19
  ret i32 %t20
}

@$main = alias i32 (), ptr @fn.0
