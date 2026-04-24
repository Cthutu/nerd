main :: fn () => ${
    break 7
}
¬
7
¬
¬
fn main
local $0 = i32:0
block
$0 = i32:7
jump L1
end
label L1
return i32:$0
end
¬
void init() {}
int $main() {
    int $0 = 0;
    {
        $0 = 7;
        goto L1;
    }
    L1: ;
    return $0;
}
