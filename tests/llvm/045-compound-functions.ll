convert :: fn { convert_i32 convert_string }
convert_i32 :: fn (value: i32) -> i32 { return value + 1 }
convert_string :: fn (value: string) -> i32 { return value.count.as(i32) }
main :: fn () -> i32 { return convert(4) }
¬
define internal i32 @fn.0(i32 %value) {
define internal i32 @fn.2() {
  %t0 = call i32 @fn.0(i32 4)
