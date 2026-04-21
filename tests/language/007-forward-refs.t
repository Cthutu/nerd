magic_number :: answer / 6
answer :: 42

main :: fn () => magic_number
¬
7
¬
¬
global answer
global magic_number
init
$0 = 42
answer = $0
$1 = answer
$2 = 6
$3 = $1 / $2
magic_number = $3
end
fn main
$0 = magic_number
return $0
end
¬
int $answer;
int $magic_number;
int init() {
    int $0 = 42;
    $answer = $0;
    int $1 = $answer;
    int $2 = 6;
    int $3 = $1 / $2;
    $magic_number = $3;
    return 0;
}
int $main() {
    int $0 = $magic_number;
    return $0;
}
