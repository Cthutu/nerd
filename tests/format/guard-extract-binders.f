choose::fn(value:?i32)->i32{
on no=>return 0
on value=>[number]{return number}
return -1
}
¬
choose :: fn (value: ?i32) -> i32 {
    on no => return 0
    on value => [number] return number
    return -1
}
