main::fn(input:string){offset:=0
for offset<input.count&&(input[offset]>=0){offset+=1}
for offset<input.count&&(is_lower(input[offset])||is_upper(input[offset])||input[offset]=='_'||is_digit(input[offset])){offset+=1}
for offset<input.count&&
input[offset]!='\0'{offset+=1}}
¬
main :: fn (input: string) {
    offset := 0
    for offset < input.count && (input[offset] >= 0) {
        offset += 1
    }
    for offset < input.count &&
        (is_lower(input[offset]) ||
            is_upper(input[offset]) ||
            input[offset] == '_' ||
            is_digit(input[offset])) {
        offset += 1
    }
    for offset < input.count &&
        input[offset] != '\0' {
        offset += 1
    }
}
