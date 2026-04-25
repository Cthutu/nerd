-- Uses a type alias and narrows a value with an explicit cast.
Price :: u16

main :: fn () {
    amount: Price = 1000
    narrowed := amount.as(u8)
    return narrowed
}
¬
232
¬

¬
fn main
local amount = u16:1000
$0 = cast u16:amount
local narrowed = u8:$0
return u8:narrowed
end
¬
void init() {}
uint8_t $main() {
    uint16_t $amount = 1000;
    uint8_t $0 = (uint8_t)$amount;
    uint8_t $narrowed = $0;
    return $narrowed;
}
