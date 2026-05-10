split :: fn (s: string, sep: string) -> [..]string {
    parts: [..]string

    on sep.count == 0 => {
        parts.push(s)
        return parts
    }

    return parts
}

main :: fn () -> i32 {
    parts := split("look", "")
    on parts.count != 1 => return 1
    on parts.count == 2 => {
        return 2
    } else return 0
    return 0
}
¬
0
¬
¬
delete
¬
--llvm-backend
