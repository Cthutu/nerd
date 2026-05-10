split :: fn (s: string, sep: string) -> [..]string {
    parts: [..]string

    on sep.count == 0 => {
        parts.push(s)
        return parts
    }

    start: usize = 0
    i: usize = 0
    for i + sep.count <= s.count {
        matched := yes
        for j: usize = 0; matched && j < sep.count; j += 1 {
            on s.data[i + j] != sep.data[j] => matched = no
        }

        on matched => {
            parts.push(s[start .. i])
            i += sep.count
            start = i
        } else {
            i += 1
        }
    }

    parts.push(s[start ..])
    return parts
}

main :: fn () -> i32 {
    parts := split("look around", " ")
    on parts.count != 2 => return 1
    on parts[0] != "look" => return 2
    on parts[1] != "around" => return 3
    return 0
}
¬
0
¬
¬
delete
¬
--llvm-backend
