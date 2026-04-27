use std.io

libm :: "m"
square_root :: ffi libm sqrt (f64) -> f64

main :: fn() {
    value := square_root(9.0)
    prn($"sqrt = {value}")
}
¬
0
¬
sqrt = 3

¬
global libm
fn main
string.reset
$0 = call fn(f64)->f64:sqrt, f64:9.0
local value = f64:$0
$1 = string.start
string.append string:"sqrt = "
string.append f64:value
$2 = string.finish $1
call fn(string)->void:prn, string:$2
string.reset
return i32:0
end
init
libm = string:"m"
end
¬
double sqrt(double);

string $libm;
int $main() {
    string_builder_reset();
    double $0 = sqrt(9.0);
    double $value = $0;
    size_t $1 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"sqrt = ", .count = 7}));
    string_builder_append_string(to_string$f64($value));
    string $2 = string_builder_finish($1);
    prn($2);
    string_builder_reset();
    return 0;
}
void init() {
    $libm = (string){.data = (u8*)"m", .count = 1};
}
