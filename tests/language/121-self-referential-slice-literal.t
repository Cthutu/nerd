Node :: plex {
    value i32
    next  ^Node
}

nodes: []Node = [
    { value: 1, next: ^nodes[1] },
    { value: 2, ... },
]

main :: fn () -> i32 {
    return nodes[0].next.value
}
¬
2
¬

¬
hir 0
bind Node = type.0
bind nodes = value.0
bind main = fn.0
type type.0 = Node
global value.0: []Node = []Node array(Node plex(value: i32 1, next: ^Node address_of(Node index([]Node bind.1(nodes), untyped integer 1))), Node plex(value: i32 2, ...))
func fn.0() -> i32 {
  return i32 field(^Node field(Node index([]Node bind.1(nodes), untyped integer 0), next), value)
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
  %t3 = insertvalue { i32, ptr } poison, i32 2, 0
  %t4 = insertvalue { i32, ptr } %t3, ptr null, 1
  %t5 = insertvalue [2 x { i32, ptr }] poison, { i32, ptr } %t2, 0
  %t6 = insertvalue [2 x { i32, ptr }] %t5, { i32, ptr } %t4, 1
  store [2 x { i32, ptr }] %t6, ptr @.slice.literal.m0.0
  %t7 = getelementptr inbounds [2 x { i32, ptr }], ptr @.slice.literal.m0.0, i64 0, i64 0
  %t8 = insertvalue { ptr, i64 } poison, ptr %t7, 0
  %t9 = insertvalue { ptr, i64 } %t8, i64 2, 1
  store { ptr, i64 } %t9, ptr @$nodes
  ret void
}

define internal i32 @fn.0() {
  %t0 = load { ptr, i64 }, ptr @$nodes
  %t1 = extractvalue { ptr, i64 } %t0, 0
  %t2 = getelementptr inbounds { i32, ptr }, ptr %t1, i32 0
  %t3 = load { i32, ptr }, ptr %t2
  %t4 = extractvalue { i32, ptr } %t3, 1
  %t5 = load { i32, ptr }, ptr %t4
  %t6 = extractvalue { i32, ptr } %t5, 0
  ret i32 %t6
}

@$main = alias i32 (), ptr @fn.0
