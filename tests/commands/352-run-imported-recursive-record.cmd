use test.recursive_message_owner

main :: fn () -> i32 {
    owner := Owner.create()
    owner.set(42)
    assert owner.message.read() == 42
    return owner.get() - 42
}
¬
0
¬

¬
delete
¬
