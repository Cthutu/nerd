use std.io

-- Uses expression blocks as top-level constant and variable initialisers.
constant_value :: ${
    break 4
}

variable_value := ${
    break constant_value + 6
}

main :: fn () {
    prn($"constant = {constant_value}")
    prn($"variable = {variable_value}")

    variable_value += 2
    prn($"updated = {variable_value}")

    return variable_value
}
¬
12
¬
constant = 4
variable = 10
updated = 12

¬
global constant_value
global variable_value
fn main
string.reset
$0 = string.start
string.append string:"constant = "
string.append i32:constant_value
$1 = string.finish $0
call fn(string)->void:prn, string:$1
string.reset
$2 = string.start
string.append string:"variable = "
string.append i32:variable_value
$3 = string.finish $2
call fn(string)->void:prn, string:$3
string.reset
$4 = i32:variable_value + i32:2
variable_value = i32:$4
$5 = string.start
string.append string:"updated = "
string.append i32:variable_value
$6 = string.finish $5
call fn(string)->void:prn, string:$6
string.reset
return i32:variable_value
end
init
local $0 = i32:0
block
$0 = i32:4
jump L1
end
label L1
constant_value = untyped-integer:$0
local $2 = i32:0
block
$4 = i32:constant_value + i32:6
$2 = i32:$4
jump L3
end
label L3
variable_value = i32:$2
end
¬
int $constant_value;
int $variable_value;
int $main() {
    string_builder_reset();
    size_t $0 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"constant = ", .count = 11}));
    string_builder_append_string(to_string$i32($constant_value));
    string $1 = string_builder_finish($0);
    prn($1);
    string_builder_reset();
    size_t $2 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"variable = ", .count = 11}));
    string_builder_append_string(to_string$i32($variable_value));
    string $3 = string_builder_finish($2);
    prn($3);
    string_builder_reset();
    int $4 = $variable_value + 2;
    $variable_value = $4;
    size_t $5 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"updated = ", .count = 10}));
    string_builder_append_string(to_string$i32($variable_value));
    string $6 = string_builder_finish($5);
    prn($6);
    string_builder_reset();
    return $variable_value;
}
void init() {
    int $0 = 0;
    {
        $0 = 4;
        goto L1;
    }
    L1: ;
    $constant_value = $0;
    int $2 = 0;
    {
        int $4 = $constant_value + 6;
        $2 = $4;
        goto L3;
    }
    L3: ;
    $variable_value = $2;
}
