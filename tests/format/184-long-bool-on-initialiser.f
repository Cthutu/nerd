main::fn(){colour:=on(pixel&(1<<(7-bit.as(u8))))!=0=>ZXColours[attr&0x07]else ZXColours[(attr>>3)&0x07]}
¬
main :: fn () {
    colour := on (pixel & (1 << (7 - bit.as(u8)))) != 0
        => ZXColours[attr & 0x07]
        else ZXColours[(attr >> 3) & 0x07]
}
