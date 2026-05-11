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

@$direct = internal global [2 x { i32, ptr }] zeroinitializer
@$aliased = internal global [2 x { i32, ptr }] zeroinitializer

define void @m0.init() {
  %t0 = getelementptr inbounds [2 x { i32, ptr }], ptr @$direct, i64 0, i32 1
  %t1 = insertvalue { i32, ptr } poison, i32 1, 0
  %t2 = insertvalue { i32, ptr } %t1, ptr %t0, 1
  %t3 = insertvalue { i32, ptr } poison, i32 2, 0
  %t4 = insertvalue { i32, ptr } %t3, ptr null, 1
  %t5 = insertvalue [2 x { i32, ptr }] poison, { i32, ptr } %t2, 0
  %t6 = insertvalue [2 x { i32, ptr }] %t5, { i32, ptr } %t4, 1
  store [2 x { i32, ptr }] %t6, ptr @$direct
  %t7 = getelementptr inbounds [2 x { i32, ptr }], ptr @$aliased, i64 0, i32 1
  %t8 = insertvalue { i32, ptr } poison, i32 3, 0
  %t9 = insertvalue { i32, ptr } %t8, ptr %t7, 1
  %t10 = getelementptr inbounds [2 x { i32, ptr }], ptr @$aliased, i64 0, i32 0
  %t11 = insertvalue { i32, ptr } poison, i32 4, 0
  %t12 = insertvalue { i32, ptr } %t11, ptr %t10, 1
  %t13 = insertvalue [2 x { i32, ptr }] poison, { i32, ptr } %t9, 0
  %t14 = insertvalue [2 x { i32, ptr }] %t13, { i32, ptr } %t12, 1
  store [2 x { i32, ptr }] %t14, ptr @$aliased
  ret void
}

define i32 @fn.0() {
  %t0 = load [2 x { i32, ptr }], ptr @$direct
  %t1 = extractvalue [2 x { i32, ptr }] %t0, 0
  %t2 = extractvalue { i32, ptr } %t1, 1
  %t3 = load { i32, ptr }, ptr %t2
  %t4 = extractvalue { i32, ptr } %t3, 0
  %t5 = load [2 x { i32, ptr }], ptr @$aliased
  %t6 = extractvalue [2 x { i32, ptr }] %t5, 0
  %t7 = extractvalue { i32, ptr } %t6, 1
  %t8 = load { i32, ptr }, ptr %t7
  %t9 = extractvalue { i32, ptr } %t8, 0
  %t10 = add i32 %t4, %t9
  %t11 = load [2 x { i32, ptr }], ptr @$aliased
  %t12 = extractvalue [2 x { i32, ptr }] %t11, 1
  %t13 = extractvalue { i32, ptr } %t12, 1
  %t14 = load { i32, ptr }, ptr %t13
  %t15 = extractvalue { i32, ptr } %t14, 0
  %t16 = add i32 %t10, %t15
  ret i32 %t16
}

@$main = alias i32 (), ptr @fn.0
