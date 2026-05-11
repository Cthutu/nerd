Node :: plex {
    value i32
    next  ^Node
}

nodes: []Node = [
    { value: 1, next: second },
    { value: 2, next: first },
]

first  :: ^nodes[0]
second :: ^nodes[1]

main :: fn () -> i32 {
    return nodes[0].next.value + nodes[1].next.value
}
¬
3
¬

¬
hir 0
bind Node = type.0
bind nodes = value.0
bind first = value.1
bind second = value.2
bind main = fn.0
type type.0 = Node
global value.0: []Node = []Node array(Node plex(value: i32 1, next: ^Node decl.3(second)), Node plex(value: i32 2, next: ^Node decl.2(first)))
const value.1: ^Node = ^Node address_of(Node index([]Node bind.1(nodes), untyped integer 0))
const value.2: ^Node = ^Node address_of(Node index([]Node bind.1(nodes), untyped integer 1))
func fn.0() -> i32 {
  return i32 add(i32 field(^Node field(Node index([]Node bind.1(nodes), untyped integer 0), next), value), i32 field(^Node field(Node index([]Node bind.1(nodes), untyped integer 1), next), value))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.slice.literal.m0.0 = private global [2 x { i32, ptr }] zeroinitializer
@$nodes = internal global { ptr, i64 } zeroinitializer

define void @m0.init() {
  %t0 = getelementptr inbounds { i32, ptr }, ptr @.slice.literal.m0.0, i32 1
  %t1 = insertvalue { i32, ptr } poison, i32 1, 0
  %t2 = insertvalue { i32, ptr } %t1, ptr %t0, 1
  %t3 = getelementptr inbounds { i32, ptr }, ptr @.slice.literal.m0.0, i32 0
  %t4 = insertvalue { i32, ptr } poison, i32 2, 0
  %t5 = insertvalue { i32, ptr } %t4, ptr %t3, 1
  %t6 = insertvalue [2 x { i32, ptr }] poison, { i32, ptr } %t2, 0
  %t7 = insertvalue [2 x { i32, ptr }] %t6, { i32, ptr } %t5, 1
  store [2 x { i32, ptr }] %t7, ptr @.slice.literal.m0.0
  %t8 = getelementptr inbounds [2 x { i32, ptr }], ptr @.slice.literal.m0.0, i64 0, i64 0
  %t9 = insertvalue { ptr, i64 } poison, ptr %t8, 0
  %t10 = insertvalue { ptr, i64 } %t9, i64 2, 1
  store { ptr, i64 } %t10, ptr @$nodes
  ret void
}

define i32 @fn.0() {
  %t0 = load { ptr, i64 }, ptr @$nodes
  %t1 = extractvalue { ptr, i64 } %t0, 0
  %t2 = getelementptr inbounds { i32, ptr }, ptr %t1, i32 0
  %t3 = load { i32, ptr }, ptr %t2
  %t4 = extractvalue { i32, ptr } %t3, 1
  %t5 = load { i32, ptr }, ptr %t4
  %t6 = extractvalue { i32, ptr } %t5, 0
  %t7 = load { ptr, i64 }, ptr @$nodes
  %t8 = extractvalue { ptr, i64 } %t7, 0
  %t9 = getelementptr inbounds { i32, ptr }, ptr %t8, i32 1
  %t10 = load { i32, ptr }, ptr %t9
  %t11 = extractvalue { i32, ptr } %t10, 1
  %t12 = load { i32, ptr }, ptr %t11
  %t13 = extractvalue { i32, ptr } %t12, 0
  %t14 = add i32 %t6, %t13
  ret i32 %t14
}

@$main = alias i32 (), ptr @fn.0
