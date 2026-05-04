use test.parts

main :: fn () -> i32 {
    return part_answer()
}
¬
42
¬
¬
fn make_thing
param i32:value
$0 = plex(value: i32:value)
return plex{value:i32}:$0
end
fn part_answer
$0 = call fn(i32)->plex{value:i32}:make_thing, i32:42
local thing = plex{value:i32}:$0
$1 = plex{value:i32}:thing.value
return i32:$1
end
fn main
$0 = call fn()->i32:part_answer
return i32:$0
end
¬
int $part_answer() {
    int $1 = $thing.$value;
    return $1;
}
int $main() {
    int $0 = $part_answer();
    return $0;
}
