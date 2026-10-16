pub count:i32=0

pub inferred:=1

pub zero:i32

pub uninitialised:i32=undefined

pub typed_constant:i32:2

pub constant::3

private_count:i32=4

private_inferred:=5

pub grouped:i32=6
private_grouped:i32=7

on "windows"{pub guarded:i32=8}

on "linux"{pub inferred_guard:=9}
¬
pub count: i32 = 0

pub inferred := 1

pub zero: i32

pub uninitialised: i32 = undefined

pub typed_constant : i32 : 2

pub constant :: 3

private_count: i32 = 4

private_inferred := 5

pub grouped     : i32 = 6
private_grouped : i32 = 7

on "windows" {

    pub guarded : i32 = 8

}

on "linux" {

    pub inferred_guard := 9

}
