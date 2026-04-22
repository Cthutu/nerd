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
$0 = cast u8:amount
local narrowed = $0
return narrowed
end
¬
void init() {}
uint8_t $main() {
    uint16_t $amount = 1000;
    uint8_t $0 = (uint8_t)$amount;
    uint8_t $narrowed = $0;
    return $narrowed;
}
