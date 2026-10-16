Node :: plex {
    value i32
    next  ^Node
}

direct: [2]Node = [
    { value: 1, next: ^direct[1] },
    { value: 2, ... },
]

aliased: [2]Node = [
    { value: 3, next: second },
    { value: 4, next: first },
]

first  :: ^aliased[0]
second :: ^aliased[1]

main :: fn () -> i32 {
    return direct[0].next.value + aliased[0].next.value + aliased[1].next.value
}
¬
9
¬

¬
hir 0
bind Node = type.0
bind direct = value.0
bind aliased = value.1
bind first = value.2
bind second = value.3
bind main = fn.0
type type.0 = Node
global value.0: [2]Node = [2]Node array(Node plex(value: i32 1, next: ^Node address_of(Node index([2]Node bind.1(direct), untyped integer 1))), Node plex(value: i32 2, ...))
global value.1: [2]Node = [2]Node array(Node plex(value: i32 3, next: ^Node decl.4(second)), Node plex(value: i32 4, next: ^Node decl.3(first)))
const value.2: ^Node = ^Node address_of(Node index([2]Node bind.2(aliased), untyped integer 0))
const value.3: ^Node = ^Node address_of(Node index([2]Node bind.2(aliased), untyped integer 1))
func fn.0() -> i32 {
  return i32 add(i32 add(i32 field(^Node field(Node index([2]Node bind.1(direct), untyped integer 0), next), value), i32 field(^Node field(Node index([2]Node bind.2(aliased), untyped integer 0), next), value)), i32 field(^Node field(Node index([2]Node bind.2(aliased), untyped integer 1), next), value))
}
¬
; nerd llvm-ir 0
; generated from HIR

@$direct = internal global [2 x { ptr, i32 }] zeroinitializer
@$aliased = internal global [2 x { ptr, i32 }] zeroinitializer

define void @m0.init() {
  %t0 = getelementptr inbounds [2 x { ptr, i32 }], ptr @$direct, i64 0, i32 1
  %t1 = insertvalue { ptr, i32 } poison, i32 1, 1
  %t2 = insertvalue { ptr, i32 } %t1, ptr %t0, 0
  %t3 = insertvalue { ptr, i32 } poison, i32 2, 1
  %t4 = insertvalue { ptr, i32 } %t3, ptr null, 0
  %t5 = insertvalue [2 x { ptr, i32 }] poison, { ptr, i32 } %t2, 0
  %t6 = insertvalue [2 x { ptr, i32 }] %t5, { ptr, i32 } %t4, 1
  store [2 x { ptr, i32 }] %t6, ptr @$direct
  %t7 = getelementptr inbounds [2 x { ptr, i32 }], ptr @$aliased, i64 0, i32 1
  %t8 = insertvalue { ptr, i32 } poison, i32 3, 1
  %t9 = insertvalue { ptr, i32 } %t8, ptr %t7, 0
  %t10 = getelementptr inbounds [2 x { ptr, i32 }], ptr @$aliased, i64 0, i32 0
  %t11 = insertvalue { ptr, i32 } poison, i32 4, 1
  %t12 = insertvalue { ptr, i32 } %t11, ptr %t10, 0
  %t13 = insertvalue [2 x { ptr, i32 }] poison, { ptr, i32 } %t9, 0
  %t14 = insertvalue [2 x { ptr, i32 }] %t13, { ptr, i32 } %t12, 1
  store [2 x { ptr, i32 }] %t14, ptr @$aliased
  ret void
}

define internal i32 @fn.0() {
  %t0 = load [2 x { ptr, i32 }], ptr @$direct
  %t1 = extractvalue [2 x { ptr, i32 }] %t0, 0
  %t2 = extractvalue { ptr, i32 } %t1, 0
  %t3 = load { ptr, i32 }, ptr %t2
  %t4 = extractvalue { ptr, i32 } %t3, 1
  %t5 = load [2 x { ptr, i32 }], ptr @$aliased
  %t6 = extractvalue [2 x { ptr, i32 }] %t5, 0
  %t7 = extractvalue { ptr, i32 } %t6, 0
  %t8 = load { ptr, i32 }, ptr %t7
  %t9 = extractvalue { ptr, i32 } %t8, 1
  %t10 = add i32 %t4, %t9
  %t11 = load [2 x { ptr, i32 }], ptr @$aliased
  %t12 = extractvalue [2 x { ptr, i32 }] %t11, 1
  %t13 = extractvalue { ptr, i32 } %t12, 0
  %t14 = load { ptr, i32 }, ptr %t13
  %t15 = extractvalue { ptr, i32 } %t14, 1
  %t16 = add i32 %t10, %t15
  ret i32 %t16
}

@$main = hidden alias i32 (), ptr @fn.0
