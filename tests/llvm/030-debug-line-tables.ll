-- test-llvm-debug: yes
main :: fn () -> i32 {
    base := 40
    bonus := 2
    total := base + bonus
    return total
}
¬
define internal i32 @fn.0() !dbg !6 {
  call void @llvm.dbg.value(metadata i32 40, metadata !10, metadata !5), !dbg !8
  call void @llvm.dbg.value(metadata i32 2, metadata !12, metadata !5), !dbg !11
  %t0 = add i32 40, 2, !dbg !13
  call void @llvm.dbg.value(metadata i32 %t0, metadata !14, metadata !5), !dbg !13
  ret i32 %t0, !dbg !15
}

@$main = alias i32 (), ptr @fn.0

declare void @llvm.dbg.value(metadata, metadata, metadata)

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4}
!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "Nerd", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "030-debug-line-tables.input.n", directory: "__REPO__/tests/llvm")
!2 = !{}
!5 = !DIExpression()
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 2, !"Dwarf Version", i32 5}
!7 = !DISubroutineType(types: !2)
!6 = distinct !DISubprogram(name: "main", linkageName: "main", scope: !1, file: !1, line: 3, type: !7, scopeLine: 3, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!8 = !DILocation(line: 3, column: 1, scope: !6)
!9 = !DIBasicType(name: "i32", size: 32, encoding: DW_ATE_signed)
!10 = !DILocalVariable(name: "base", scope: !6, file: !1, line: 1, type: !9)
!11 = !DILocation(line: 4, column: 1, scope: !6)
!12 = !DILocalVariable(name: "bonus", scope: !6, file: !1, line: 1, type: !9)
!13 = !DILocation(line: 5, column: 1, scope: !6)
!14 = !DILocalVariable(name: "total", scope: !6, file: !1, line: 1, type: !9)
!15 = !DILocation(line: 6, column: 1, scope: !6)
