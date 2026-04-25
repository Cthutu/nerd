use mod std.print

Value :: union {
    i i32
    f f32
}

main :: fn () -> i32 {
    a: Value = Value { i: 42 }
    b: Value = Value { f: 3.5 }

    prn($"i {a.i}")
    prn($"f {b.f}")

    return a.i
}
¬
42
¬
i 42
f 3.5

¬
fn main
string.reset
$0 = union(i: i32:42)
local a = union{i:i32,f:f32}:$0
$1 = union(f: f32:3.5)
local b = union{i:i32,f:f32}:$1
$2 = string.start
string.append string:"i "
$4 = union{i:i32,f:f32}:a.i
string.append i32:$4
$3 = string.finish $2
call fn(string)->void:prn, string:$3
string.reset
$5 = string.start
string.append string:"f "
$7 = union{i:i32,f:f32}:b.f
string.append f32:$7
$6 = string.finish $5
call fn(string)->void:prn, string:$6
string.reset
$8 = union{i:i32,f:f32}:a.i
return i32:$8
end
¬
void init() {}
typedef union union8 {
    int $i;
    float $f;
} union8;
int $main() {
    string_builder_reset();
    union8 $0 = (union8){.$i = 42};
    union8 $a = $0;
    union8 $1 = (union8){.$f = 3.5f};
    union8 $b = $1;
    size_t $2 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"i ", .count = 2}));
    int $4 = $a.$i;
    string_builder_append_string(to_string$i32($4));
    string $3 = string_builder_finish($2);
    prn($3);
    string_builder_reset();
    size_t $5 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"f ", .count = 2}));
    float $7 = $b.$f;
    string_builder_append_string(to_string$f32($7));
    string $6 = string_builder_finish($5);
    prn($6);
    string_builder_reset();
    int $8 = $a.$i;
    return $8;
}
