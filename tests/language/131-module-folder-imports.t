use test.folder_mod
priority :: use test.folder_priority

main :: fn () -> i32 {
    return answer() + priority.priority_answer()
}
¬
49
¬
¬
fn answer
return i32:42
end
fn main
$0 = call fn()->i32:answer
$1 = call fn()->i32:priority_answer
$2 = i32:$0 + i32:$1
return i32:$2
end
¬
int $answer() {
    return 42;
}
int $main() {
    int $0 = $answer();
    int $1 = $priority_answer();
    int $2 = $0 + $1;
    return $2;
}
