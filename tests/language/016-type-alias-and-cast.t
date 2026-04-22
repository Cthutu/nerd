Price :: u16

main :: fn () {
    amount: Price = 1000
    narrowed := amount.cast(u8)
    return narrowed
}
¬
232
¬

¬
fn main
local amount = 1000
$0 = cast amount, type=6
local narrowed = $0
return narrowed
end
¬
void init() {}
int $main() {
    int $amount = 1000;
    int $0 = (uint8_t)$amount;
    int $narrowed = $0;
    return $narrowed;
}
