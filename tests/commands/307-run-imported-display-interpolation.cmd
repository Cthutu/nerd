values :: use display_values
main :: fn () {
    value := values.Value { count: 42 }
    assert $"{value}" == "value=42"
    failure := values.failure(5)
    assert $"{failure}" == "error=5"
}
¬
0
¬
