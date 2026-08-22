main :: fn (result: string\Failure) {
    on result => [file]
    {
        prn(file)
    } else [error]
    {
        prn(error)
    }
}
¬
main :: fn (result: string\Failure) {
    on result => [file] {
        prn(file)
    } else [error] {
        prn(error)
    }
}
