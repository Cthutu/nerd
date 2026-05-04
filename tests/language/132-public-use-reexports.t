use test.folder_pub_use
folder :: use test.folder_pub_use

main :: fn () -> i32 {
    return child_answer() + folder.local_answer()
}
¬
42
¬
¬
fn child_answer
return i32:37
end
fn main
$0 = call fn()->i32:child_answer
$1 = call fn()->i32:local_answer
$2 = i32:$0 + i32:$1
return i32:$2
end
¬
int $child_answer() {
    return 37;
}
int $main() {
    int $0 = $child_answer();
    int $1 = $local_answer();
    int $2 = $0 + $1;
    return $2;
}
