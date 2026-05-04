add::fn(a:i32,b:i32=1,c:i32=a+b)=>a+b+c
blocky::fn(
first:i32,
second:i32=first+1
)->i32{
return first+second
}
main::fn(){
add(a=1,c=3)
}
¬
add :: fn (a: i32, b: i32 = 1, c: i32 = a + b) => a + b + c

blocky :: fn (first: i32, second: i32 = first + 1) -> i32 {
    return first + second
}

main :: fn () {
    add(a = 1, c = 3)
}
