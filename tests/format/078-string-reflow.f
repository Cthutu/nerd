short::"Hello, " "world!"
long_message :: "This formatter test contains a deliberately long plain string literal that should be split at spaces into adjacent string literals when it is formatted."
main::fn(){text := "alpha" " beta" " gamma"}
¬
short        :: "Hello, world!"
long_message ::
    "This formatter test contains a deliberately long plain string literal that "
    "should be split at spaces into adjacent string literals when it is "
    "formatted."

main :: fn () {
    text := "alpha beta gamma"
}
