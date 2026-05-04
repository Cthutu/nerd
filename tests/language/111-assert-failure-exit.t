-- A failing assertion prints its message and exits with status 127.
main :: fn () {
    assert no, "stopped"
}
¬
127
¬
¬
fn main
assert bool:no, string:"stopped", line 3
return i32:0
end
¬
void init() {}
int $main() {
    nerd_assert(false, "tests/language/111-assert-failure-exit.t", 3, (string){.data = (u8*)"stopped", .count = 7});
    return 0;
}
