-- test-llvm-debug: yes
saved_width: u32
saved_height: u32

main :: fn () {
    saved_width = 41
    saved_height = 19
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [36 x i8] c"tests/llvm/036-debug-globals.input.n\00"

@$saved_width = internal global i32 0, !dbg !9
@$saved_height = internal global i32 0, !dbg !11

define void @m0.init() {
  ret void
}

define internal void @fn.0() !dbg !12 {
  store i32 41, ptr @$saved_width, !dbg !14
  store i32 19, ptr @$saved_height, !dbg !15
  ret void, !dbg !15
}

@$main = alias void (), ptr @fn.0

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "Nerd", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, globals: !6)
!1 = !DIFile(filename: "036-debug-globals.input.n", directory: "__REPO__/tests/llvm")
!2 = !{}
!5 = !DIExpression()
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 2, !"Dwarf Version", i32 5}
!7 = !DIBasicType(name: "u32", size: 32, encoding: DW_ATE_unsigned)
!8 = distinct !DIGlobalVariable(name: "saved_width", scope: !0, file: !1, line: 1, type: !7, isLocal: false, isDefinition: true)
!9 = !DIGlobalVariableExpression(var: !8, expr: !5)
!10 = distinct !DIGlobalVariable(name: "saved_height", scope: !0, file: !1, line: 1, type: !7, isLocal: false, isDefinition: true)
!11 = !DIGlobalVariableExpression(var: !10, expr: !5)
!13 = !DISubroutineType(types: !2)
!12 = distinct !DISubprogram(name: "main", linkageName: "main", scope: !1, file: !1, line: 6, type: !13, scopeLine: 6, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!14 = !DILocation(line: 6, column: 1, scope: !12)
!15 = !DILocation(line: 7, column: 1, scope: !12)
!6 = !{!9, !11}
