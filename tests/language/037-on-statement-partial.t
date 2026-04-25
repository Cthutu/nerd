use mod std.print

-- Statement-position block-form `on` may omit `else`.
main :: fn () {
    value :: 2

    on value {
        1 => prn("one")
    }

    prn("done")
}
¬
0
¬
done

¬
fn main
$2 = i32:2 == i32:1
branch.false bool:$2, L0
label L1
call fn(string)->void:prn, string:"one"
label L0
call fn(string)->void:prn, string:"done"
return i32:0
end
¬
void init() {}
int $main() {
    bool $2 = 2 == 1;
    if (!$2) goto L0;
    L1: ;
    prn((string){.data = (u8*)"one", .count = 3});
    L0: ;
    prn((string){.data = (u8*)"done", .count = 4});
    return 0;
}
