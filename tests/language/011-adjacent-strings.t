use mod std.print

-- Concatenates adjacent string literals at compile time.
main :: fn () {
    prn("Hello, " "world!")
}
¬
0
¬
Hello, world!

¬
fn main
call fn(string)->void:prn, string:"Hello, world!"
return i32:0
end
¬
void init() {}
int $main() {
    prn((string){.data = (u8*)"Hello, world!", .count = 13});
    return 0;
}
