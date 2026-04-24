main :: fn () {
    first :: ${
        break 7
    }
    second :: ${
        break first + 5
    }
    word :: ${
        break "done"
    }
    flag :: ${
        break yes
    }

    pr("first = ")
    prn($"{first}")
    prn($"second = {second}")
    prn($"word = {word}")
    prn($"flag = {flag}")
    return second
}
¬
12
¬
first = 7
second = 12
word = done
flag = yes

¬
fn main
string.reset
call fn(string)->void:pr, string:"first = "
$0 = string.start
local $2 = i32:0
block
$2 = i32:7
jump L3
end
label L3
string.append i32:$2
$1 = string.finish $0
call fn(string)->void:prn, string:$1
string.reset
$4 = string.start
string.append string:"second = "
local $6 = i32:0
block
$8 = i32:$2 + i32:5
$6 = i32:$8
jump L7
end
label L7
string.append i32:$6
$5 = string.finish $4
call fn(string)->void:prn, string:$5
string.reset
$9 = string.start
string.append string:"word = "
local $11 = string:0
block
$11 = string:"done"
jump L12
end
label L12
string.append string:$11
$10 = string.finish $9
call fn(string)->void:prn, string:$10
string.reset
$13 = string.start
string.append string:"flag = "
local $15 = bool:no
block
$15 = bool:yes
jump L16
end
label L16
string.append bool:$15
$14 = string.finish $13
call fn(string)->void:prn, string:$14
string.reset
return i32:$6
end
¬
void init() {}
int $main() {
    string_builder_reset();
    pr((string){.data = (u8*)"first = ", .count = 8});
    size_t $0 = string_builder_mark();
    int $2 = 0;
    {
        $2 = 7;
        goto L3;
    }
    L3: ;
    string_builder_append_string(to_string$i32($2));
    string $1 = string_builder_finish($0);
    prn($1);
    string_builder_reset();
    size_t $4 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"second = ", .count = 9}));
    int $6 = 0;
    {
        int $8 = $2 + 5;
        $6 = $8;
        goto L7;
    }
    L7: ;
    string_builder_append_string(to_string$i32($6));
    string $5 = string_builder_finish($4);
    prn($5);
    string_builder_reset();
    size_t $9 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"word = ", .count = 7}));
    string $11 = (string){0};
    {
        $11 = (string){.data = (u8*)"done", .count = 4};
        goto L12;
    }
    L12: ;
    string_builder_append_string(to_string$string($11));
    string $10 = string_builder_finish($9);
    prn($10);
    string_builder_reset();
    size_t $13 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"flag = ", .count = 7}));
    bool $15 = false;
    {
        $15 = true;
        goto L16;
    }
    L16: ;
    string_builder_append_string(to_string$bool($15));
    string $14 = string_builder_finish($13);
    prn($14);
    string_builder_reset();
    return $6;
}
