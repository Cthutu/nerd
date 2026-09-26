use test.named_defaults

add :: fn (a: i32, b: i32 = 2, c: i32 = a + b) => a + b + c
identity :: fn [T] (value: T, enabled: bool = yes) => value
specialised :: fn (value: i32, enabled :: bool = yes) => on enabled => value else 0
Counter :: plex { value i32 }
impl Counter {
    add :: fn (self: Self, amount: i32 = 1) => self.value + amount
}
select_i32 :: fn (value: i32, increment: i32 = 1) => value + increment
select_bool :: fn (value: bool) => on value => 1 else 0
select :: fn {
    select_i32
    select_bool
}
invoke :: fn (callback: fn (a: i32, b: i32, c: i32) -> i32) => callback(1, 2, 3)
main :: fn () -> i32 {
    on add(3) != 10 => return 1
    on add(3, b = 4) != 14 => return 2
    on add(3, b = 4, c = 8) != 15 => return 3
    alias := add
    on alias(3, b = 4) != 14 => return 4
    on identity(7, enabled = no) != 7 => return 5
    on specialised(8, enabled = no) != 0 => return 6
    counter := Counter { value: 10 }
    on counter.add(amount = 3) != 13 => return 7
    on select(10, increment = 4) != 14 => return 8
    on imported(10, increment = 5) != 15 => return 9
    on imported_generic(6, enabled = no) != 6 => return 10
    on invoke(add) != 6 => return 11
    return 0
}
¬
0
¬
