-- test-llvm-debug: yes
add :: fn (left: i32, right: i32) -> i32 {
    total := left + right
    return total
}

main :: fn () -> i32 {
    return add(20, 22)
}
¬
define internal i32 @fn.0(i32 %left, i32 %right) !dbg !6 {
  call void @llvm.dbg.value(metadata i32 %left, metadata !9, metadata !5), !dbg !10
  call void @llvm.dbg.value(metadata i32 %right, metadata !11, metadata !5), !dbg !10
  %t0 = add i32 %left, %right, !dbg !12
  call void @llvm.dbg.value(metadata i32 %t0, metadata !13, metadata !5), !dbg !12
  ret i32 %t0, !dbg !14
}

declare void @llvm.dbg.value(metadata, metadata, metadata)

!llvm.dbg.cu = !{!0}
!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "Nerd", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "031-debug-parameters.input.n", directory: "__REPO__/tests/llvm")
!5 = !DIExpression()
!6 = distinct !DISubprogram(name: "add", linkageName: "@fn.0", scope: !1, file: !1, line: 3, type: !7, scopeLine: 3, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!8 = !DIBasicType(name: "i32", size: 32, encoding: DW_ATE_signed)
!9 = !DILocalVariable(name: "left", scope: !6, file: !1, line: 2, type: !8)
!11 = !DILocalVariable(name: "right", scope: !6, file: !1, line: 2, type: !8)
!12 = !DILocation(line: 3, column: 1, scope: !6)
!13 = !DILocalVariable(name: "total", scope: !6, file: !1, line: 1, type: !8)
!14 = !DILocation(line: 4, column: 1, scope: !6)
