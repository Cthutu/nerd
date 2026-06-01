main :: fn () -> i32 {
    text := """
        alpha
          beta
        gamma
        """

    expected := "alpha\n  beta\ngamma\n"
    on text != expected => return 1

    compact := """one
    two"""
    on compact != "one\n    two" => return 2

    escaped := """
        quote: \"""
        slash: \\
        """
    on escaped != "quote: \"\"\"\nslash: \\\n" => return 3

    return 0
}
¬
0
¬

¬
