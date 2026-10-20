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
define internal i32 @fn.0({ [4 x i32] } %cell) {
  %t0 = extractvalue { [4 x i32] } %cell, 0
  %t1 = extractvalue [4 x i32] %t0, 1
  %t2 = extractvalue { [4 x i32] } %cell, 0
  %t3 = extractvalue [4 x i32] %t2, 2
  %t4 = add i32 %t1, %t3
  ret i32 %t4
}

define internal i32 @fn.1() {
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
  %t15 = extractvalue [4 x i32] %t14, 1
  %t16 = load { { [4 x i32] } }, ptr %local.1
  %t17 = extractvalue { { [4 x i32] } } %t16, 0
  %t18 = extractvalue { [4 x i32] } %t17, 0
  %t19 = extractvalue [4 x i32] %t18, 2
  %t20 = add i32 %t15, %t19
  %t21 = add i32 %t20, %t11
  ret i32 %t21
}
