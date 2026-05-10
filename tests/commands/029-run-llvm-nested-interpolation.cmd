use std.io

warm :: "warm"
cool :: "cool"
unknown :: "unknown"

classify :: fn (choice: string) -> i32 {
    return on choice {
        "red", "green" => 1
        "blue" => 2
        else => 3
    }
}

label :: fn (choice: string) -> string {
    return on choice {
        "red" => warm
        "blue" => cool
        else => unknown
    }
}

main :: fn () {
    prn($"red: {classify("red")} {label("red")}")
    prn($"blue: {classify("blue")} {label("blue")}")
    return classify("blue")
}
¬
2
¬
red: 1 warm
blue: 2 cool

¬
delete
¬
