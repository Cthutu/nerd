forward :: fn (callback: fn (arg1: i32, ...) -> i32, args: ...) -> i32 {
    return callback(args.next[i32]())
}
¬
forward :: fn (callback: fn (arg1: i32, ...) -> i32, args: ...) => callback(args.next[i32]())
