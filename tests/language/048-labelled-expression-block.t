use mod std.print

main :: fn () {
    answer :: $answer {
        break $answer 41 + 1
    }
    word :: $word {
        break $word "labelled"
    }
    outer :: $outer {
        $inner {
            break $outer 99
        }
        break $outer 0
    }
    hits := 0
    $void {
        hits += 1
        break $void
    }

    prn($"answer = {answer}")
    prn($"word = {word}")
    prn($"outer = {outer}")
    prn($"hits = {hits}")
    return outer
}
¬
99
¬
answer = 42
word = labelled
outer = 99
hits = 1

¬
fn main
string.reset
local hits = i32:0
block
$1 = i32:hits + i32:1
hits = i32:$1
jump L0
end
label L0
$2 = string.start
string.append string:"answer = "
local $4 = i32:0
block
$4 = i32:42
jump L5
end
label L5
string.append i32:$4
$3 = string.finish $2
call fn(string)->void:prn, string:$3
string.reset
$6 = string.start
string.append string:"word = "
local $8 = string:0
block
$8 = string:"labelled"
jump L9
end
label L9
string.append string:$8
$7 = string.finish $6
call fn(string)->void:prn, string:$7
string.reset
$10 = string.start
string.append string:"outer = "
local $12 = i32:0
block
block
$12 = i32:99
jump L13
end
label L14
$12 = i32:0
jump L13
end
label L13
string.append i32:$12
$11 = string.finish $10
call fn(string)->void:prn, string:$11
string.reset
$15 = string.start
string.append string:"hits = "
string.append i32:hits
$16 = string.finish $15
call fn(string)->void:prn, string:$16
string.reset
return i32:$12
end
¬
void init() {}
int $main() {
    string_builder_reset();
    int $hits = 0;
    {
        int $1 = $hits + 1;
        $hits = $1;
        goto L0;
    }
    L0: ;
    size_t $2 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"answer = ", .count = 9}));
    int $4 = 0;
    {
        $4 = 42;
        goto L5;
    }
    L5: ;
    string_builder_append_string(to_string$i32($4));
    string $3 = string_builder_finish($2);
    prn($3);
    string_builder_reset();
    size_t $6 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"word = ", .count = 7}));
    string $8 = (string){0};
    {
        $8 = (string){.data = (u8*)"labelled", .count = 8};
        goto L9;
    }
    L9: ;
    string_builder_append_string(to_string$string($8));
    string $7 = string_builder_finish($6);
    prn($7);
    string_builder_reset();
    size_t $10 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"outer = ", .count = 8}));
    int $12 = 0;
    {
        {
            $12 = 99;
            goto L13;
        }
        L14: ;
        $12 = 0;
        goto L13;
    }
    L13: ;
    string_builder_append_string(to_string$i32($12));
    string $11 = string_builder_finish($10);
    prn($11);
    string_builder_reset();
    size_t $15 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"hits = ", .count = 7}));
    string_builder_append_string(to_string$i32($hits));
    string $16 = string_builder_finish($15);
    prn($16);
    string_builder_reset();
    return $12;
}
