-- Decodes underscores in float literals as digit separators.
main :: fn () -> i32 {
    value := 1_0.5 + 3_1.5
    return value.as(i32)
}
¬
42
¬
¬
fn main
$0 = f64:10.5 + f64:31.5
local value = f64:$0
$1 = cast f64:value
return i32:$1
end
¬
void init() {}
int $main() {
    double $0 = 10.5 + 31.5;
    double $value = $0;
    int $1 = (int)$value;
    return $1;
}
