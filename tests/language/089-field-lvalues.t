Node :: plex {
    value i32
}

main :: fn () -> i32 {
    node: Node = Node { value: 1 }
    node.value = 3
    node.value += 2
    node.value *= 2
    node.value /= 2
    node.value -= 1

    ptr: ^Node = ^node
    ptr.value = 5
    ptr.value += 3
    ptr^.value = 7
    ptr^.value += 4
    ptr^.value %= 6

    return node.value + ptr^.value - 10
}
¬
0
¬

¬
hir 0
bind Node = type.0
bind main = fn.0
type type.0 = Node
func fn.0() -> i32 {
  let node: Node = Node plex(value: i32 1)
  assign i32 field(Node local.0(node), value) = i32 3
  assign i32 field(Node local.0(node), value) = i32 add(i32 field(Node local.0(node), value), i32 2)
  assign i32 field(Node local.0(node), value) = i32 multiply(i32 field(Node local.0(node), value), i32 2)
  assign i32 field(Node local.0(node), value) = i32 divide(i32 field(Node local.0(node), value), i32 2)
  assign i32 field(Node local.0(node), value) = i32 subtract(i32 field(Node local.0(node), value), i32 1)
  let ptr: ^Node = ^Node address_of(Node local.0(node))
  assign i32 field(^Node local.1(ptr), value) = i32 5
  assign i32 field(^Node local.1(ptr), value) = i32 add(i32 field(^Node local.1(ptr), value), i32 3)
  assign i32 field(Node deref(^Node local.1(ptr)), value) = i32 7
  assign i32 field(Node deref(^Node local.1(ptr)), value) = i32 add(i32 field(Node deref(^Node local.1(ptr)), value), i32 4)
  assign i32 field(Node deref(^Node local.1(ptr)), value) = i32 modulo(i32 field(Node deref(^Node local.1(ptr)), value), i32 6)
  return i32 subtract(i32 add(i32 field(Node local.0(node), value), i32 field(Node deref(^Node local.1(ptr)), value)), i32 10)
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %t0 = insertvalue { i32 } poison, i32 1, 0
  %local.0 = alloca { i32 }
  store { i32 } %t0, ptr %local.0
  %t1 = getelementptr inbounds { i32 }, ptr %local.0, i64 0, i32 0
  store i32 3, ptr %t1
  %t2 = load { i32 }, ptr %local.0
  %t3 = extractvalue { i32 } %t2, 0
  %t4 = add i32 %t3, 2
  %t5 = getelementptr inbounds { i32 }, ptr %local.0, i64 0, i32 0
  store i32 %t4, ptr %t5
  %t6 = load { i32 }, ptr %local.0
  %t7 = extractvalue { i32 } %t6, 0
  %t8 = mul i32 %t7, 2
  %t9 = getelementptr inbounds { i32 }, ptr %local.0, i64 0, i32 0
  store i32 %t8, ptr %t9
  %t10 = load { i32 }, ptr %local.0
  %t11 = extractvalue { i32 } %t10, 0
  %t12 = sdiv i32 %t11, 2
  %t13 = getelementptr inbounds { i32 }, ptr %local.0, i64 0, i32 0
  store i32 %t12, ptr %t13
  %t14 = load { i32 }, ptr %local.0
  %t15 = extractvalue { i32 } %t14, 0
  %t16 = sub i32 %t15, 1
  %t17 = getelementptr inbounds { i32 }, ptr %local.0, i64 0, i32 0
  store i32 %t16, ptr %t17
  %t18 = getelementptr inbounds { i32 }, ptr %local.0, i64 0, i32 0
  store i32 5, ptr %t18
  %t19 = load { i32 }, ptr %local.0
  %t20 = extractvalue { i32 } %t19, 0
  %t21 = add i32 %t20, 3
  %t22 = getelementptr inbounds { i32 }, ptr %local.0, i64 0, i32 0
  store i32 %t21, ptr %t22
  %t23 = getelementptr inbounds { i32 }, ptr %local.0, i64 0, i32 0
  store i32 7, ptr %t23
  %t24 = load { i32 }, ptr %local.0
  %t25 = extractvalue { i32 } %t24, 0
  %t26 = add i32 %t25, 4
  %t27 = getelementptr inbounds { i32 }, ptr %local.0, i64 0, i32 0
  store i32 %t26, ptr %t27
  %t28 = load { i32 }, ptr %local.0
  %t29 = extractvalue { i32 } %t28, 0
  %t30 = srem i32 %t29, 6
  %t31 = getelementptr inbounds { i32 }, ptr %local.0, i64 0, i32 0
  store i32 %t30, ptr %t31
  %t32 = load { i32 }, ptr %local.0
  %t33 = extractvalue { i32 } %t32, 0
  %t34 = load { i32 }, ptr %local.0
  %t35 = extractvalue { i32 } %t34, 0
  %t36 = add i32 %t33, %t35
  %t37 = sub i32 %t36, 10
  ret i32 %t37
}

@$main = alias i32 (), ptr @fn.0
