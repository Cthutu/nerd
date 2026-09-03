use test.generics

pick_local :: fn [T] (values: []T, fallback: T) -> T {
    on values.count == 0 => return fallback
    return values[1]
}

main :: fn () -> i32 {
    values := [10, 20, 30]
    local_fixed    := pick_local(values, 0)
    local_empty    := pick_local([], 22)
    imported_fixed := pick(values, 0)
    imported_empty := pick([], 22)
    return local_fixed + local_empty + imported_fixed + imported_empty
}
¬
84
¬
¬
¬
