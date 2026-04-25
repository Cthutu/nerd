Maybe::enum{None Some(i32) Pair(i32,i32) Text(string)}

score::fn(value:Maybe)->i32{
return on value{
None=>0
Some(x)=>x
Pair(left,right)=>left+right
Text(_)=>100
}
}
¬
Maybe :: enum { None Some(i32) Pair(i32, i32) Text(string) }

score :: fn (value: Maybe) -> i32 {
    return on value { None => 0 Some(x) => x Pair(left, right) => left + right Text(_) => 100 }
}
