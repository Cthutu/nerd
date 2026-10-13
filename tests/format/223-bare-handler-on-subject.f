main::fn(){for{on shutdown.requested() else [error]{log(error)
return 1}=>break
on ready()=>value() else fallback()
on yes=>on requested() else {return}=>7 else 9
}}
¬
main :: fn () {
    for {
        on shutdown.requested() else [error] {
            log(error)
            return 1
        } => break
        on ready() => value() else fallback()
        on yes => on requested() else {
            return
        } => 7 else 9
    }
}
