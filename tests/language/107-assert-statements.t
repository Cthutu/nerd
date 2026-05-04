-- Checks passing assertions with and without an explicit message.
main :: fn () -> i32 {
    assert 2 < 3
    assert 4 == 4, "math still works"
    return 0
}
¬
0
¬
¬
fn main
$0 = i32:2 < i32:3
assert bool:$0, string:"assertion failed", line 3
$1 = i32:4 == i32:4
assert bool:$1, string:"math still works", line 4
return i32:0
end
¬
void init() {}
int $main() {
    bool $0 = 2 < 3;
    nerd_assert($0, "tests/language/107-assert-statements.t", 3, (string){.data = (u8*)"assertion failed", .count = 16});
    bool $1 = 4 == 4;
    nerd_assert($1, "tests/language/107-assert-statements.t", 4, (string){.data = (u8*)"math still works", .count = 16});
    return 0;
}
