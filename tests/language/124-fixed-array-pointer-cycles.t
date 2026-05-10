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

@$direct = global [2 x { i32, ptr }] zeroinitializer
@$aliased = global [2 x { i32, ptr }] zeroinitializer

define void @m0.init() {
  %t0 = getelementptr inbounds [2 x { i32, ptr }], ptr @$direct, i64 0, i32 1
  %t1 = insertvalue { i32, ptr } poison, i32 1, 0
  %t2 = insertvalue { i32, ptr } %t1, ptr %t0, 1
  %t3 = insertvalue { i32, ptr } poison, i32 2, 0
  %t4 = insertvalue { i32, ptr } %t3, ptr null, 1
  %t5 = insertvalue [2 x { i32, ptr }] poison, { i32, ptr } %t2, 0
  %t6 = insertvalue [2 x { i32, ptr }] %t5, { i32, ptr } %t4, 1
  store [2 x { i32, ptr }] %t6, ptr @$direct
  ret void
}

define i32 @fn.0() {
  %t0 = load [2 x { i32, ptr }], ptr @$direct
  %t1 = getelementptr inbounds [2 x { i32, ptr }], ptr @$direct, i64 0, i32 0
  %t2 = load { i32, ptr }, ptr %t1
  %t3 = extractvalue { i32, ptr } %t2, 1
  %t4 = load { i32, ptr }, ptr %t3
  %t5 = extractvalue { i32, ptr } %t4, 0
  %t6 = load [2 x { i32, ptr }], ptr @$aliased
  %t7 = getelementptr inbounds [2 x { i32, ptr }], ptr @$aliased, i64 0, i32 0
  %t8 = load { i32, ptr }, ptr %t7
  %t9 = extractvalue { i32, ptr } %t8, 1
  %t10 = load { i32, ptr }, ptr %t9
  %t11 = extractvalue { i32, ptr } %t10, 0
  %t12 = add i32 %t5, %t11
  %t13 = load [2 x { i32, ptr }], ptr @$aliased
  %t14 = getelementptr inbounds [2 x { i32, ptr }], ptr @$aliased, i64 0, i32 1
  %t15 = load { i32, ptr }, ptr %t14
  %t16 = extractvalue { i32, ptr } %t15, 1
  %t17 = load { i32, ptr }, ptr %t16
  %t18 = extractvalue { i32, ptr } %t17, 0
  %t19 = add i32 %t12, %t18
  ret i32 %t19
}

@$main = alias i32 (), ptr @fn.0