base :: 4
factor :: 5
answer :: base * factor + 2

main :: fn () => answer
¬
22
¬
¬
global base
global factor
global answer
init
base = 4
factor = 5
answer = 22
end
fn main
return 22
end
¬
int $base;
int $factor;
int $answer;
int init() {
    $base = 4;
    $factor = 5;
    $answer = 22;
    return 0;
}
int $main() {
    return 22;
}
