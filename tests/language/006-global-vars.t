answer :: 42
magic_number :: 7

main :: fn () => answer / magic_number
¬
6
¬
¬
global answer
global magic_number
init
answer = 42
magic_number = 7
end
fn main
return 6
end
¬
int $answer;
int $magic_number;
int init() {
    $answer = 42;
    $magic_number = 7;
    return 0;
}
int $main() {
    return 6;
}
