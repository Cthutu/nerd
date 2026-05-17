first :: fn [T] (iter: T) -> Option[i32]
where T: Iterator[i32] {
    return None
}

main :: fn () -> i32 {
    _value: Option[i32] = Some(21)
    _status: Result[i32, string] = Ok(21)
    return 42
}
¬
42
¬

¬
hir 0
¬
