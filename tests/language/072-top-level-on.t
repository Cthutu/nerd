on debug {
    use std.io
    answer :: 7

    on !feature {
        fallback :: 9
    }
}

main :: fn () {
    prn($"answer={answer}")
    prn($"fallback={fallback}")
}
¬
0
¬
answer=7
fallback=9

¬
fn main
call fn(string)->void:prn, string:"answer=7"
call fn(string)->void:prn, string:"fallback=9"
return i32:0
end
¬
void init() {}
int $main() {
    prn((string){.data = (u8*)"answer=7", .count = 8});
    prn((string){.data = (u8*)"fallback=9", .count = 10});
    return 0;
}
