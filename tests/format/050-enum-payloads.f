Maybe::enum{None Some(i32) Pair(i32,i32) Text(string)}

score::fn(value:Maybe)->i32{
return on value{
None=>0
Maybe.Some(as x)=>x
Pair(as left,as right)=>left+right
Text(_)=>100
}
}
¬
Maybe :: enum {
    None
    Some(i32)
    Pair(i32, i32)
    Text(string)
}

score :: fn (value: Maybe) -> i32 {
    return on value {
        None => 0
        Maybe.Some(as x) => x
        Pair(as left, as right) => left + right
        Text(_) => 100
    }
}
