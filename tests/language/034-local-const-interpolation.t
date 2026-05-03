use std.io

main :: fn () {
  i :: 2
  f :: 3.14
  s :: "Hello, world!"

  prn($"{s}  i = {i} and f = {f}!")
}
¬
0
¬
Hello, world!  i = 2 and f = 3.14!

¬
fn main
call fn(string)->void:prn, string:"Hello, world!  i = 2 and f = 3.14!"
return i32:0
end
¬
void init() {}
int $main() {
    prn((string){.data = (u8*)"Hello, world!  i = 2 and f = 3.14!", .count = 34});
    return 0;
}
