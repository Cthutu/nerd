Cell :: plex {
    values [4]i32
}

Box :: plex {
    cell Cell
}

take_cell :: fn (cell: Cell) -> i32 {
    return cell.values[1] + cell.values[2]
}

main :: fn () -> i32 {
    box: Box
    box.cell.values[1] = 7
    (^box.cell.values[2])^ = 9

    direct := take_cell(Cell { values: [1, 2, 3, 4] })
    return box.cell.values[1] + box.cell.values[2] + direct
}
¬
21
¬

¬
hir 0
bind Cell = type.0
bind Box = type.1
bind take_cell = fn.0
bind main = fn.1
type type.0 = Cell
type type.1 = Box
func fn.0(cell: Cell) -> i32 {
  return i32 add(i32 index([4]i32 field(Cell local.0(cell), values), untyped integer 1), i32 index([4]i32 field(Cell local.0(cell), values), untyped integer 2))
}
func fn.1() -> i32 {
  expr <unknown> <unsupported>
  let box: Box = <unknown> <unsupported>
  assign i32 index([4]i32 field(Cell field(Box local.1(box), cell), values), untyped integer 1) = i32 7
  assign i32 deref(^i32 address_of(i32 index([4]i32 field(Cell field(Box local.1(box), cell), values), untyped integer 2))) = i32 9
  let direct: i32 = i32 call bind.2(take_cell)(Cell plex(values: [4]i32 array(i32 1, i32 2, i32 3, i32 4)))
  return i32 add(i32 add(i32 index([4]i32 field(Cell field(Box local.1(box), cell), values), untyped integer 1), i32 index([4]i32 field(Cell field(Box local.1(box), cell), values), untyped integer 2)), i32 local.2(direct))
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0({ [4 x i32] } %cell) {
  %t0 = extractvalue { [4 x i32] } %cell, 0
  %local.0 = alloca { [4 x i32] }
  store { [4 x i32] } %cell, ptr %local.0
  %t1 = getelementptr inbounds { [4 x i32] }, ptr %local.0, i64 0, i32 0
  %t2 = getelementptr inbounds [4 x i32], ptr %t1, i64 0, i32 1
  %t3 = load i32, ptr %t2
  %t4 = load { [4 x i32] }, ptr %local.0
  %t5 = extractvalue { [4 x i32] } %t4, 0
  %t6 = getelementptr inbounds { [4 x i32] }, ptr %local.0, i64 0, i32 0
  %t7 = getelementptr inbounds [4 x i32], ptr %t6, i64 0, i32 2
  %t8 = load i32, ptr %t7
  %t9 = add i32 %t3, %t8
  ret i32 %t9
}

define i32 @fn.1() {
  %local.1 = alloca { { [4 x i32] } }
  store { { [4 x i32] } } zeroinitializer, ptr %local.1
  %t0 = getelementptr inbounds { { [4 x i32] } }, ptr %local.1, i64 0, i32 0
  %t1 = getelementptr inbounds { [4 x i32] }, ptr %t0, i64 0, i32 0
  %t2 = getelementptr inbounds [4 x i32], ptr %t1, i64 0, i32 1
  store i32 7, ptr %t2
  %t3 = getelementptr inbounds { { [4 x i32] } }, ptr %local.1, i64 0, i32 0
  %t4 = getelementptr inbounds { [4 x i32] }, ptr %t3, i64 0, i32 0
  %t5 = getelementptr inbounds [4 x i32], ptr %t4, i64 0, i32 2
  store i32 9, ptr %t5
  %t6 = insertvalue [4 x i32] poison, i32 1, 0
  %t7 = insertvalue [4 x i32] %t6, i32 2, 1
  %t8 = insertvalue [4 x i32] %t7, i32 3, 2
  %t9 = insertvalue [4 x i32] %t8, i32 4, 3
  %t10 = insertvalue { [4 x i32] } poison, [4 x i32] %t9, 0
  %t11 = call i32 @fn.0({ [4 x i32] } %t10)
  %t12 = load { { [4 x i32] } }, ptr %local.1
  %t13 = extractvalue { { [4 x i32] } } %t12, 0
  %t14 = extractvalue { [4 x i32] } %t13, 0
  %t15 = getelementptr inbounds { { [4 x i32] } }, ptr %local.1, i64 0, i32 0
  %t16 = getelementptr inbounds { [4 x i32] }, ptr %t15, i64 0, i32 0
  %t17 = getelementptr inbounds [4 x i32], ptr %t16, i64 0, i32 1
  %t18 = load i32, ptr %t17
  %t19 = load { { [4 x i32] } }, ptr %local.1
  %t20 = extractvalue { { [4 x i32] } } %t19, 0
  %t21 = extractvalue { [4 x i32] } %t20, 0
  %t22 = getelementptr inbounds { { [4 x i32] } }, ptr %local.1, i64 0, i32 0
  %t23 = getelementptr inbounds { [4 x i32] }, ptr %t22, i64 0, i32 0
  %t24 = getelementptr inbounds [4 x i32], ptr %t23, i64 0, i32 2
  %t25 = load i32, ptr %t24
  %t26 = add i32 %t18, %t25
  %t27 = add i32 %t26, %t11
  ret i32 %t27
}

@$take_cell = alias i32 ({ [4 x i32] }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1