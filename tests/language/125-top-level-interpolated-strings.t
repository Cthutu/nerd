name :: "world"
count :: 40 + 2
truth :: yes
greeting :: $"Hello, {name}! count={count}, ok={truth}"

make :: fn () -> string {
    return $"constant {name} {count}"
}

main :: fn () => ((greeting != "Hello, world! count=42, ok=yes") ||
                  (make() != "constant world 42")).as(i32)
¬
0
¬
¬
global name
global greeting
fn make
return string:"constant world 42"
end
fn main
local $0 = bool:no
$4 = string:greeting != string:"Hello, world! count=42, ok=yes"
branch.false bool:$4, L1
$0 = bool:yes
jump L3
label L1
$5 = call fn()->string:make
$6 = string:$5 != string:"constant world 42"
branch.false bool:$6, L2
$0 = bool:yes
jump L3
label L2
$0 = bool:no
label L3
$7 = cast bool:$0
return i32:$7
end
init
name = string:"world"
greeting = string:"Hello, world! count=42, ok=yes"
end
¬
string $name;
string $greeting;
string $make() {
    return (string){.data = (u8*)"constant world 42", .count = 17};
}
int $main() {
    bool $0 = false;
    bool $4 = !string_eq($greeting, (string){.data = (u8*)"Hello, world! count=42, ok=yes", .count = 30});
    if (!$4) goto L1;
    $0 = true;
    goto L3;
    L1: ;
    string $5 = $make();
    bool $6 = !string_eq($5, (string){.data = (u8*)"constant world 42", .count = 17});
    if (!$6) goto L2;
    $0 = true;
    goto L3;
    L2: ;
    $0 = false;
    L3: ;
    int $7 = (int)$0;
    return $7;
}
void init() {
    $name = (string){.data = (u8*)"world", .count = 5};
    $greeting = (string){.data = (u8*)"Hello, world! count=42, ok=yes", .count = 30};
}
