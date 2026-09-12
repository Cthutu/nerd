forward :: fn (callback: fn (count: i32, args: ...) -> i32, args: ...) -> i32 {
    return callback(args.next[i32]())
}
¬
forward :: fn (callback: fn (count: i32, args: ...) -> i32, args: ...) => callback(args.next[i32]())
