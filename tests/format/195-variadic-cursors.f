sum :: fn(count:i32,args:...)->i32 { return count+args.next[i32]() }
Callback :: fn(arg1: i32,...)->i32
¬
sum :: fn (count: i32, args: ...) -> i32 {
    return count + args.next[i32]()
}
Callback :: fn (arg1: i32, ...) -> i32
