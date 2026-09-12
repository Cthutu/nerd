c_abs :: ffi "c" abs (value: i32) -> i32
pub abs :: fn (value: i32) -> i32 { return c_abs(value) }
¬
1
¬

¬
delete
¬
--dll
¬
build
¬
runtime-error: C export `abs` collides with an imported foreign symbol; choose a
               distinct public binding name
