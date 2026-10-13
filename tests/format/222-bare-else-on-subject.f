main::fn(){
for{
on (shutdown.requested() else [error]{
eprn($"Shutdown watcher failed: {error}")
return 1
})=>break
on ((optional() else {return}) && ready)=>break
on ready && (optional() else {return})=>break
}
}
¬
main :: fn () {
    for {
        on (shutdown.requested() else [error] {
            eprn($"Shutdown watcher failed: {error}")
            return 1
        }) => break
        on ((optional() else {
            return
        }) && ready) => break
        on ready &&
            (optional() else {
                return
            }) => break
    }
}
