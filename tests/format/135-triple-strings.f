main :: fn () {
text := """alpha
beta"""
escaped := "alpha\n  beta\n"
}
¬
main :: fn () {
    text := """
        alpha
        beta
        """
    escaped := """
        alpha
          beta
        """
}
