forward :: fn (callback: fn (i32, ...) -> i32, args: ...) -> i32 {
    return callback(args.next[i32]())
}
¬
forward :: fn (callback: fn (i32, ...) -> i32, args: ...) => callback(args.next[i32]())
