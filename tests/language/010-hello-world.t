main :: fn () {
    prn("Hello, world!")
}
¬
0
¬
Hello, world!

¬
fn main
call prn, "Hello, world!"
return i32:0
end
¬
void init() {}
int $main() {
    prn((string){.data = (u8*)"Hello, world!", .count = 13});
    return 0;
}
