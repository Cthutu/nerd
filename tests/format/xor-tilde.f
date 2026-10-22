main::fn(){
a:u32=7
b:u32=3
bits:=a~~b|~a&b
bits~=~a
pointer:=^bits
pointer^=9
pointer^~=b
copy:=pointer^-1
again_value:=pointer ^ -1
result:=callback^(pointer^~b)
result2:=callback_ptr^^(-1)
}
¬
main :: fn () {
    a    : u32 = 7
    b    : u32 = 3
    bits       := a ~ ~b | ~a & b

    bits ~= ~a
    pointer := ^bits
    pointer^ = 9
    pointer^ ~= b
    copy        := pointer^ - 1
    again_value := pointer^ - 1
    result      := callback^(pointer^ ~ b)
    result2     := callback_ptr^^(-1)
}
