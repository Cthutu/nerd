-- Binds public declarations directly to types and uses them as aliases.
pub Score :: i32
pub FinalScore :: Score

add :: fn (lhs: Score, rhs: FinalScore) -> Score {
    return lhs + rhs
}

main :: fn () {
    lhs: Score = 20
    rhs: FinalScore = 22
    return add(lhs, rhs)
}
¬
42
¬

¬
fn add
param i32:lhs
param i32:rhs
$0 = i32:lhs + i32:rhs
return i32:$0
end
fn main
local lhs = i32:20
local rhs = i32:22
$0 = call fn(i32,i32)->i32:add, i32:lhs, i32:rhs
return i32:$0
end
¬
void init() {}
int $add(int $lhs, int $rhs) {
    int $0 = $lhs + $rhs;
    return $0;
}
int $main() {
    int $lhs = 20;
    int $rhs = 22;
    int $0 = $add($lhs, $rhs);
    return $0;
}
