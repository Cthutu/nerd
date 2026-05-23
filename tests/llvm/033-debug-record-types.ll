-- test-llvm-debug: yes
Point :: plex {
    x i32
    y i32
}

main :: fn () {
    message: string
    message = "hello"
    point: Point
    point = Point { x: 3 y: 4 }
    on point.x == 3 => prn(message)
}
¬
define internal void @fn.0() !dbg !6 {
  %local.0 = alloca { ptr, i64 }
  call void @llvm.dbg.declare(metadata ptr %local.0, metadata !15, metadata !5), !dbg !16
  %local.1 = alloca { i32, i32 }
  call void @llvm.dbg.declare(metadata ptr %local.1, metadata !24, metadata !5), !dbg !16
}

!9 = !DICompositeType(tag: DW_TAG_structure_type, name: "string", file: !1, size: 128, elements: !10)
!10 = !{!11, !12}
!13 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "ptr", baseType: null, size: 64)
!11 = !DIDerivedType(tag: DW_TAG_member, name: "data", scope: !9, file: !1, baseType: !13, size: 64, offset: 0)
!14 = !DIBasicType(name: "usize", size: 64, encoding: DW_ATE_unsigned)
!12 = !DIDerivedType(tag: DW_TAG_member, name: "count", scope: !9, file: !1, baseType: !14, size: 64, offset: 64)
!15 = !DILocalVariable(name: "message", scope: !6, file: !1, line: 1, type: !9)
!19 = !DICompositeType(tag: DW_TAG_structure_type, name: "Point", file: !1, size: 64, elements: !20)
!20 = !{!21, !22}
!23 = !DIBasicType(name: "i32", size: 32, encoding: DW_ATE_signed)
!21 = !DIDerivedType(tag: DW_TAG_member, name: "x", scope: !19, file: !1, baseType: !23, size: 32, offset: 0)
!22 = !DIDerivedType(tag: DW_TAG_member, name: "y", scope: !19, file: !1, baseType: !23, size: 32, offset: 32)
!24 = !DILocalVariable(name: "point", scope: !6, file: !1, line: 1, type: !19)
