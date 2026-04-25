main :: fn () -> i32 {
    text :: "hello"
    whole: string = text[..]
    ell: string = text[1..4]
    tail: string = text[2..]
    head: string = text[..2]

    prn($"whole = {whole}")
    prn($"ell = {ell}")
    prn($"tail = {tail}")
    prn($"head = {head}")
    prn($"count = {ell.count}")
    prn($"first byte = {ell.data[0]}")

    result :: on ell {
        "ell" => 7
        else => 1
    }

    return result + ell.data[1].as(i32)
}
¬
115
¬
whole = hello
ell = ell
tail = llo
head = he
count = 3
first byte = 101

¬
fn main
string.reset
$0 = string:"hello"[..]
local whole = string:$0
$1 = string:"hello"[i32:1..i32:4]
local ell = string:$1
$2 = string:"hello"[i32:2..]
local tail = string:$2
$3 = string:"hello"[..i32:2]
local head = string:$3
$4 = string.start
string.append string:"whole = "
string.append string:whole
$5 = string.finish $4
call fn(string)->void:prn, string:$5
string.reset
$6 = string.start
string.append string:"ell = "
string.append string:ell
$7 = string.finish $6
call fn(string)->void:prn, string:$7
string.reset
$8 = string.start
string.append string:"tail = "
string.append string:tail
$9 = string.finish $8
call fn(string)->void:prn, string:$9
string.reset
$10 = string.start
string.append string:"head = "
string.append string:head
$11 = string.finish $10
call fn(string)->void:prn, string:$11
string.reset
$12 = string.start
string.append string:"count = "
$14 = string:ell.count
string.append usize:$14
$13 = string.finish $12
call fn(string)->void:prn, string:$13
string.reset
$15 = string.start
string.append string:"first byte = "
$17 = string:ell.data
$18 = ^u8:$17[i32:0]
string.append u8:$18
$16 = string.finish $15
call fn(string)->void:prn, string:$16
string.reset
local $19 = i32:0
$23 = string:ell == string:"ell"
branch.false bool:$23, L21
label L22
$19 = i32:7
jump L20
label L21
$19 = i32:1
label L20
$24 = string:ell.data
$25 = ^u8:$24[i32:1]
$26 = cast u8:$25
$27 = i32:$19 + i32:$26
return i32:$27
end
¬
void init() {}
int $main() {
    string_builder_reset();
    string $0 = string_slice((string){.data = (u8*)"hello", .count = 5}, 0, (string){.data = (u8*)"hello", .count = 5}.count);
    string $whole = $0;
    string $1 = string_slice((string){.data = (u8*)"hello", .count = 5}, 1, 4);
    string $ell = $1;
    string $2 = string_slice((string){.data = (u8*)"hello", .count = 5}, 2, (string){.data = (u8*)"hello", .count = 5}.count);
    string $tail = $2;
    string $3 = string_slice((string){.data = (u8*)"hello", .count = 5}, 0, 2);
    string $head = $3;
    size_t $4 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"whole = ", .count = 8}));
    string_builder_append_string(to_string$string($whole));
    string $5 = string_builder_finish($4);
    prn($5);
    string_builder_reset();
    size_t $6 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"ell = ", .count = 6}));
    string_builder_append_string(to_string$string($ell));
    string $7 = string_builder_finish($6);
    prn($7);
    string_builder_reset();
    size_t $8 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"tail = ", .count = 7}));
    string_builder_append_string(to_string$string($tail));
    string $9 = string_builder_finish($8);
    prn($9);
    string_builder_reset();
    size_t $10 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"head = ", .count = 7}));
    string_builder_append_string(to_string$string($head));
    string $11 = string_builder_finish($10);
    prn($11);
    string_builder_reset();
    size_t $12 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"count = ", .count = 8}));
    uintptr_t $14 = $ell.count;
    string_builder_append_string(to_string$usize($14));
    string $13 = string_builder_finish($12);
    prn($13);
    string_builder_reset();
    size_t $15 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"first byte = ", .count = 13}));
    uint8_t* $17 = $ell.data;
    uint8_t $18 = $17[0];
    string_builder_append_string(to_string$u8($18));
    string $16 = string_builder_finish($15);
    prn($16);
    string_builder_reset();
    int $19 = 0;
    bool $23 = string_eq($ell, (string){.data = (u8*)"ell", .count = 3});
    if (!$23) goto L21;
    L22: ;
    $19 = 7;
    goto L20;
    L21: ;
    $19 = 1;
    L20: ;
    uint8_t* $24 = $ell.data;
    uint8_t $25 = $24[1];
    int $26 = (int)$25;
    int $27 = $19 + $26;
    return $27;
}
