use std.io

-- Allows a block-bodied main with no explicit return value.
main :: fn () {
    prn("Hello from void main")
}
¬
0
¬
Hello from void main

¬
fn main
call fn(string)->void:prn, string:"Hello from void main"
return i32:0
end
¬
void init() {}
int $main() {
    prn((string){.data = (u8*)"Hello from void main", .count = 20});
    return 0;
}
