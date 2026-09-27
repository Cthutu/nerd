main :: fn () {
    pr("a")
    pr("")
    prn("b")
    prn()
    prn(text = "named")
    output: fn (text: string) -> void = prn
    output("alias")
    epr("")
    eprn("error")
    eprn()
}
¬
0
¬
ab

named
alias

¬
delete
¬

¬
run
¬
error

