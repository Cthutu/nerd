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
$0 = 42
answer = $0
$1 = 7
magic_number = $1
end
fn main
$0 = answer
$1 = magic_number
$2 = $0 / $1
return $2
end
¬
int $answer;
int $magic_number;
int init() {
    int $0 = 42;
    $answer = $0;
    int $1 = 7;
    $magic_number = $1;
    return 0;
}
int $main() {
    int $0 = $answer;
    int $1 = $magic_number;
    int $2 = $0 / $1;
    return $2;
}
