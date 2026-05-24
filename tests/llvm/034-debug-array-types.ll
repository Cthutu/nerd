-- test-llvm-debug: yes
main :: fn () {
    values: [3]i32
    values = [10, 20, 30]
    on values[1] == 20 => {}
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [41 x i8] c"tests/llvm/034-debug-array-types.input.n\00"

define internal void @fn.0() !dbg !6 {
  %local.0 = alloca [3 x i32]
  call void @llvm.dbg.declare(metadata ptr %local.0, metadata !13, metadata !5), !dbg !14
  store [3 x i32] zeroinitializer, ptr %local.0, !dbg !8
  %t0 = insertvalue [3 x i32] poison, i32 10, 0, !dbg !15
  %t1 = insertvalue [3 x i32] %t0, i32 20, 1, !dbg !15
  %t2 = insertvalue [3 x i32] %t1, i32 30, 2, !dbg !15
  store [3 x i32] %t2, ptr %local.0, !dbg !15
  %t3 = load [3 x i32], ptr %local.0, !dbg !16
  %t4 = extractvalue [3 x i32] %t3, 1, !dbg !16
  %t5 = icmp eq i32 %t4, 20, !dbg !16
  %t6 = icmp eq i1 %t5, 1, !dbg !16
  br i1 %t6, label %on.body.1, label %on.end.0, !dbg !16
on.body.1:
  br label %on.end.0, !dbg !16
on.end.0:
  ret void, !dbg !16
}

@$main = alias void (), ptr @fn.0

declare void @llvm.dbg.declare(metadata, metadata, metadata)

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "Nerd", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "034-debug-array-types.input.n", directory: "__REPO__/tests/llvm")
!2 = !{}
!5 = !DIExpression()
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 2, !"Dwarf Version", i32 5}
!7 = !DISubroutineType(types: !2)
!6 = distinct !DISubprogram(name: "main", linkageName: "main", scope: !1, file: !1, line: 3, type: !7, scopeLine: 3, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!8 = !DILocation(line: 3, column: 1, scope: !6)
!10 = !DIBasicType(name: "i32", size: 32, encoding: DW_ATE_signed)
!9 = !DICompositeType(tag: DW_TAG_array_type, name: "[3]i32", file: !1, baseType: !10, size: 96, elements: !11)
!11 = !{!12}
!12 = !DISubrange(count: 3, lowerBound: 0)
!13 = !DILocalVariable(name: "values", scope: !6, file: !1, line: 1, type: !9)
!14 = !DILocation(line: 1, column: 1, scope: !6)
!15 = !DILocation(line: 4, column: 1, scope: !6)
!16 = !DILocation(line: 5, column: 1, scope: !6)
