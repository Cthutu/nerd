-- Accepts every expression-start token handled by binding classification.
floaty :: 3.14
negative :: -1
truth :: yes
inverted :: !no

main :: fn () => 0
¬
0
¬
¬
global floaty
global inverted
fn main
return i32:0
end
init
floaty = untyped-float:3.1400000000000001
$0 = !bool:no
inverted = bool:$0
end
¬
double $floaty;
bool $inverted;
int $main() {
    return 0;
}
void init() {
    $floaty = 3.1400000000000001;
    bool $0 = !0;
    $inverted = $0;
}
