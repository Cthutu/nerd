choice :: "blue"

classify :: fn (choice: string) -> i32 {
    return on choice {
        "red", "green" => 1
        "blue" => 2
        else => 3
    }
}

main :: fn () => classify(choice)
¬
2
¬

¬
delete
¬
--llvm-backend
