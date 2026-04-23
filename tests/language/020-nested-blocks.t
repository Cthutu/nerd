main :: fn () {
    value := 10
    {
        value := 32
        value = value + 1
    }
    return value
}
¬
10
¬

¬
fn main
local value = i32:10
block
local value = i32:32
$0 = i32:value + i32:1
value = i32:$0
end
return i32:value
end
¬
void init() {}
int $main() {
    int $value = 10;
    {
        int $value = 32;
        int $0 = $value + 1;
        $value = $0;
    }
    return $value;
}
