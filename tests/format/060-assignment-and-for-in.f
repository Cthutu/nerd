main::fn(text:string,cond:bool){
value:=1
ptr:^i32=^value
for ^c in text{
c^ = 1
}
on cond=>return yes
}
¬
main :: fn (text: string, cond: bool) {
    value := 1
    ptr : ^i32 = ^value
    for ^c in text {
        c^ = 1
    }
    on cond => return yes
}
