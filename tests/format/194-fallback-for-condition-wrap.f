main::fn(input:string){
session:={input ...}
offset:=0
on input[offset]{
in ['0' ..= '9']=>{
for offset < input.count && (input[offset] in ['0' ..= '9']){offset+=1}}
in ['a' ..= 'z']=>{
for offset < input.count && (input[offset] in ['a' ..= 'z'] || input[offset] in ['A' ..= 'Z'] || input[offset]=='_' || input[offset] in ['0' ..= '9']){offset+=1}}}}
¬
main :: fn (input: string) {
    session := {
        input ...
    }
    offset := 0
    on input[offset] {
        in ['0' ..= '9'] => {
            for offset < input.count &&
                (input[offset] in ['0' ..= '9']) {
                offset += 1
            }
        }
        in ['a' ..= 'z'] => {
            for offset < input.count &&
                (input[offset] in ['a' ..= 'z'] ||
                    input[offset] in ['A' ..= 'Z'] ||
                    input[offset] == '_' ||
                    input[offset] in ['0' ..= '9']) {
                offset += 1
            }
        }
    }
}
