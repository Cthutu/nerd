pub
choose :: fn (value: i32) -> i32 =>
on value > 0 => 1 else 0
short :: 1
longer_name :: 2
broken :: fn () { call(&value) }
pub
other :: fn (value: i32) -> i32 =>
on value > 0 => 1 else 0
another :: 1
longest_name :: 2
¬
pub choose :: fn (value: i32) => on value > 0 => 1 else 0

short       :: 1
longer_name :: 2

broken :: fn () {
    call(& value)
}

pub other :: fn (value: i32) => on value > 0 => 1 else 0

another      :: 1
longest_name :: 2
