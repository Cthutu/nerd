use std.print

-- Matches strings with block-form `on` branches.
choice :: "blue"
warm :: "warm"
cool :: "cool"
unknown :: "unknown"

classify :: fn (choice: string) -> i32 {
    return on choice {
        "red", "green" => 1
        "blue" => 2
        else => 3
    }
}

label :: fn (choice: string) -> string {
    return on choice {
        "red" => warm
        "blue" => cool
        else => unknown
    }
}

main :: fn () {
    prn($"red: {classify("red")} {label("red")}")
    prn($"green: {classify("green")} {label("green")}")
    prn($"blue: {classify("blue")} {label("blue")}")
    prn($"other: {classify("other")} {label("other")}")

    return classify(choice)
}
¬
2
¬
red: 1 warm
green: 1 unknown
blue: 2 cool
other: 3 unknown

¬
global choice
global warm
global cool
global unknown
fn classify
param string:choice
local $0 = i32:0
$5 = string:choice == string:"red"
branch.false bool:$5, L4
jump L3
label L4
$6 = string:choice == string:"green"
branch.false bool:$6, L2
label L3
$0 = i32:1
jump L1
label L2
$9 = string:choice == string:"blue"
branch.false bool:$9, L7
label L8
$0 = i32:2
jump L1
label L7
$0 = i32:3
label L1
return i32:$0
end
fn label
param string:choice
local $0 = string:0
$4 = string:choice == string:"red"
branch.false bool:$4, L2
label L3
$0 = string:warm
jump L1
label L2
$7 = string:choice == string:"blue"
branch.false bool:$7, L5
label L6
$0 = string:cool
jump L1
label L5
$0 = string:unknown
label L1
return string:$0
end
fn main
string.reset
$0 = string.start
string.append string:"red: "
$2 = call fn(string)->i32:classify, string:"red"
string.append i32:$2
string.append string:" "
$3 = call fn(string)->string:label, string:"red"
string.append string:$3
$1 = string.finish $0
call fn(string)->void:prn, string:$1
string.reset
$4 = string.start
string.append string:"green: "
$6 = call fn(string)->i32:classify, string:"green"
string.append i32:$6
string.append string:" "
$7 = call fn(string)->string:label, string:"green"
string.append string:$7
$5 = string.finish $4
call fn(string)->void:prn, string:$5
string.reset
$8 = string.start
string.append string:"blue: "
$10 = call fn(string)->i32:classify, string:"blue"
string.append i32:$10
string.append string:" "
$11 = call fn(string)->string:label, string:"blue"
string.append string:$11
$9 = string.finish $8
call fn(string)->void:prn, string:$9
string.reset
$12 = string.start
string.append string:"other: "
$14 = call fn(string)->i32:classify, string:"other"
string.append i32:$14
string.append string:" "
$15 = call fn(string)->string:label, string:"other"
string.append string:$15
$13 = string.finish $12
call fn(string)->void:prn, string:$13
string.reset
$16 = call fn(string)->i32:classify, string:choice
return i32:$16
end
init
choice = string:"blue"
warm = string:"warm"
cool = string:"cool"
unknown = string:"unknown"
end
¬
string $choice;
string $warm;
string $cool;
string $unknown;
int $classify(string $choice) {
    int $0 = 0;
    bool $5 = string_eq($choice, (string){.data = (u8*)"red", .count = 3});
    if (!$5) goto L4;
    goto L3;
    L4: ;
    bool $6 = string_eq($choice, (string){.data = (u8*)"green", .count = 5});
    if (!$6) goto L2;
    L3: ;
    $0 = 1;
    goto L1;
    L2: ;
    bool $9 = string_eq($choice, (string){.data = (u8*)"blue", .count = 4});
    if (!$9) goto L7;
    L8: ;
    $0 = 2;
    goto L1;
    L7: ;
    $0 = 3;
    L1: ;
    return $0;
}
string $label(string $choice) {
    string $0 = (string){0};
    bool $4 = string_eq($choice, (string){.data = (u8*)"red", .count = 3});
    if (!$4) goto L2;
    L3: ;
    $0 = $warm;
    goto L1;
    L2: ;
    bool $7 = string_eq($choice, (string){.data = (u8*)"blue", .count = 4});
    if (!$7) goto L5;
    L6: ;
    $0 = $cool;
    goto L1;
    L5: ;
    $0 = $unknown;
    L1: ;
    return $0;
}
int $main() {
    string_builder_reset();
    size_t $0 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"red: ", .count = 5}));
    int $2 = $classify((string){.data = (u8*)"red", .count = 3});
    string_builder_append_string(to_string$i32($2));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string $3 = $label((string){.data = (u8*)"red", .count = 3});
    string_builder_append_string(to_string$string($3));
    string $1 = string_builder_finish($0);
    prn($1);
    string_builder_reset();
    size_t $4 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"green: ", .count = 7}));
    int $6 = $classify((string){.data = (u8*)"green", .count = 5});
    string_builder_append_string(to_string$i32($6));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string $7 = $label((string){.data = (u8*)"green", .count = 5});
    string_builder_append_string(to_string$string($7));
    string $5 = string_builder_finish($4);
    prn($5);
    string_builder_reset();
    size_t $8 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"blue: ", .count = 6}));
    int $10 = $classify((string){.data = (u8*)"blue", .count = 4});
    string_builder_append_string(to_string$i32($10));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string $11 = $label((string){.data = (u8*)"blue", .count = 4});
    string_builder_append_string(to_string$string($11));
    string $9 = string_builder_finish($8);
    prn($9);
    string_builder_reset();
    size_t $12 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"other: ", .count = 7}));
    int $14 = $classify((string){.data = (u8*)"other", .count = 5});
    string_builder_append_string(to_string$i32($14));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string $15 = $label((string){.data = (u8*)"other", .count = 5});
    string_builder_append_string(to_string$string($15));
    string $13 = string_builder_finish($12);
    prn($13);
    string_builder_reset();
    int $16 = $classify($choice);
    return $16;
}
void init() {
    $choice = (string){.data = (u8*)"blue", .count = 4};
    $warm = (string){.data = (u8*)"warm", .count = 4};
    $cool = (string){.data = (u8*)"cool", .count = 4};
    $unknown = (string){.data = (u8*)"unknown", .count = 7};
}
