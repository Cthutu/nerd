-- test-llvm-debug: yes
Value :: union {
    i i32
    f f32
}

main :: fn () -> i32 {
    value: Value
    value = Value { i: 42 }
    number := 7
    ptr := ^number
    return ptr^ + value.i
}
¬
!9 = !DICompositeType(tag: DW_TAG_union_type, name: "Value", file: !1, size: 32, elements: !10)
!10 = !{!11, !12}
!13 = !DIBasicType(name: "i32", size: 32, encoding: DW_ATE_signed)
!11 = !DIDerivedType(tag: DW_TAG_member, name: "i", scope: !9, file: !1, baseType: !13, size: 32, offset: 0)
!14 = !DIBasicType(name: "f32", size: 32, encoding: DW_ATE_float)
!12 = !DIDerivedType(tag: DW_TAG_member, name: "f", scope: !9, file: !1, baseType: !14, size: 32, offset: 0)
!15 = !DILocalVariable(name: "value", scope: !6, file: !1, line: 1, type: !9)
!19 = !DILocalVariable(name: "number", scope: !6, file: !1, line: 1, type: !13)
!21 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "ptr", baseType: !13, size: 64)
!22 = !DILocalVariable(name: "ptr", scope: !6, file: !1, line: 1, type: !21)
