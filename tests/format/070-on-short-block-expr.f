split::fn(s:string,sep:string)->[..]string{parts:[..]string on sep.count==0=>{parts.push(s)return parts} on sep.count==1=>${break s.data[0]} return parts}
¬
split :: fn (s: string, sep: string) -> [..]string {
    parts : [..]string
    on sep.count == 0 => {
        parts.push(s)
        return parts
    }
    on sep.count == 1 => ${
        break s.data[0]
    }
    return parts
}
