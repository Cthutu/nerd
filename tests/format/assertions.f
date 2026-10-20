main::fn(){
assert 2<3
assert yes,"message"
assert reserved_size>=initial_alloc_size,$"Arena reserve size must be at least {initial_alloc_size} bytes"
assert yes,"This formatter test contains a deliberately long assert message that should move onto the next line and split into multiple strings."
}
¬
main :: fn () {
    assert 2 < 3
    assert yes, "message"
    assert reserved_size >= initial_alloc_size,
        $"Arena reserve size must be at least {initial_alloc_size} bytes"
    assert yes,
        "This formatter test contains a deliberately long assert message that "
        +"should move onto the next line and split into multiple strings."
}
¬
assert on "linux"
pub syscall::intrinsic"syscall"(number:u64,a0:u64,a1:u64,a2:u64,a3:u64,a4:u64,a5:u64)->i64
¬
assert on "linux"

pub syscall :: intrinsic "syscall" (number: u64, a0: u64, a1: u64, a2: u64, a3: u64, a4: u64, a5: u64) -> i64
