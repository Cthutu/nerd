enabled:bool=yes
main::fn()=>on enabled=>42 else 7

blocky::fn(){
on enabled=>{ return } else { return }
}
¬
enabled: bool = yes

main :: fn () => on enabled => 42 else 7

blocky :: fn () {
    on enabled => {
        return
    } else {
        return
    }
}
