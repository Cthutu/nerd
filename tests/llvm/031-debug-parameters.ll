-- test-llvm-debug: yes
add :: fn (left: i32, right: i32) -> i32 {
    total := left + right
    return total
}

main :: fn () -> i32 {
    return add(20, 22)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [40 x i8] c"tests/llvm/031-debug-parameters.input.n\00"

define internal i32 @fn.0(i32 %left, i32 %right) !dbg !6 {
  %local.0 = alloca i32
  call void @llvm.dbg.declare(metadata ptr %local.0, metadata !9, metadata !5), !dbg !10
  %local.1 = alloca i32
  call void @llvm.dbg.declare(metadata ptr %local.1, metadata !11, metadata !5), !dbg !10
  store i32 %left, ptr %local.0
  store i32 %right, ptr %local.1
  %t0 = load i32, ptr %local.0, !dbg !12
  %t1 = load i32, ptr %local.1, !dbg !12
  %t2 = add i32 %t0, %t1, !dbg !12
  call void @llvm.dbg.value(metadata i32 %t2, metadata !13, metadata !5), !dbg !12
  ret i32 %t2, !dbg !14
}

define internal i32 @fn.1() !dbg !15 {
  %t0 = call i32 @fn.0(i32 20, i32 22), !dbg !17
  ret i32 %t0, !dbg !17
}

@$add = internal alias i32 (i32, i32), ptr @fn.0
@$main = alias i32 (), ptr @fn.1

declare void @llvm.dbg.declare(metadata, metadata, metadata)
declare void @llvm.dbg.value(metadata, metadata, metadata)

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "Nerd", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "031-debug-parameters.input.n", directory: "__REPO__/tests/llvm")
!2 = !{}
!5 = !DIExpression()
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 2, !"Dwarf Version", i32 5}
!7 = !DISubroutineType(types: !2)
!6 = distinct !DISubprogram(name: "add", scope: !1, file: !1, line: 3, type: !7, scopeLine: 3, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!8 = !DIBasicType(name: "i32", size: 32, encoding: DW_ATE_signed)
!9 = !DILocalVariable(name: "left", scope: !6, file: !1, line: 2, arg: 1, type: !8)
!10 = !DILocation(line: 2, column: 1, scope: !6)
!11 = !DILocalVariable(name: "right", scope: !6, file: !1, line: 2, arg: 2, type: !8)
!12 = !DILocation(line: 3, column: 1, scope: !6)
!13 = !DILocalVariable(name: "total", scope: !6, file: !1, line: 1, type: !8)
!14 = !DILocation(line: 4, column: 1, scope: !6)
!16 = !DISubroutineType(types: !2)
!15 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 8, type: !16, scopeLine: 8, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!17 = !DILocation(line: 8, column: 1, scope: !15)
