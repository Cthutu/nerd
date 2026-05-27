main :: fn () -> usize {
    groups := [["quit", "q"]]
    result: usize = 0
    for i, group in groups {
        for item in group^ {
            on item^ == "q" => result = i + 1
        }
    }
    return result
}
¬
1
¬

¬

¬
