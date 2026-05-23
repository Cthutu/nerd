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
  call void @llvm.dbg.declare(metadata ptr %local.0, metadata !16, metadata !5), !dbg !17
  %local.1 = alloca { i32, i32 }
  call void @llvm.dbg.declare(metadata ptr %local.1, metadata !25, metadata !5), !dbg !17
}

!9 = !DICompositeType(tag: DW_TAG_structure_type, name: "string", file: !1, size: 128, elements: !10)
!10 = !{!11, !12}
!14 = !DIBasicType(name: "u8", size: 8, encoding: DW_ATE_unsigned)
!13 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "ptr", baseType: !14, size: 64)
!11 = !DIDerivedType(tag: DW_TAG_member, name: "data", scope: !9, file: !1, baseType: !13, size: 64, offset: 0)
!15 = !DIBasicType(name: "usize", size: 64, encoding: DW_ATE_unsigned)
!12 = !DIDerivedType(tag: DW_TAG_member, name: "count", scope: !9, file: !1, baseType: !15, size: 64, offset: 64)
!16 = !DILocalVariable(name: "message", scope: !6, file: !1, line: 1, type: !9)
!20 = !DICompositeType(tag: DW_TAG_structure_type, name: "Point", file: !1, size: 64, elements: !21)
!21 = !{!22, !23}
!24 = !DIBasicType(name: "i32", size: 32, encoding: DW_ATE_signed)
!22 = !DIDerivedType(tag: DW_TAG_member, name: "x", scope: !20, file: !1, baseType: !24, size: 32, offset: 0)
!23 = !DIDerivedType(tag: DW_TAG_member, name: "y", scope: !20, file: !1, baseType: !24, size: 32, offset: 32)
!25 = !DILocalVariable(name: "point", scope: !6, file: !1, line: 1, type: !20)
