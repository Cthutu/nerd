lex::fn(input:string){offset:=0
for offset<input.count{on input[offset]{in [0..=9]=>{session.tokens.push({
kind:TokenType.Integer
offset:start
}
)}}}}
¬
lex :: fn (input: string) {
    offset := 0
    for offset < input.count {
        on input[offset] {
            in [0 ..= 9] => {
                session.tokens.push({
                    kind: TokenType.Integer
                    offset: start
                })
            }
        }
    }
}
