main::fn(){
value:u32=0x1234
text:=$"{value>>8&0xFF:02X}{value&0xFF:02X}"+"/{value:8d}"
}
¬
main :: fn () {
    value : u32 = 0x1234
    text        := $"{value >> 8 & 0xFF:02X}{value & 0xFF:02X}/{value:8d}"
}
