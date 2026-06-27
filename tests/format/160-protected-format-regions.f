main::fn(){
values:=[]i32{
--[
        1,2,3,
              4,   5,   6,
    ? this would not lex normally
--]
}
return
}
¬
main :: fn () {
    values := []i32 {
--[
        1,2,3,
              4,   5,   6,
    ? this would not lex normally
--]
    }
    return
}
