main::fn(){
a:=on yes=>(optional() else {return 1}) else 3
b:=on yes=>identity(optional() else {return 2}) else 3
c:=on operation()=>[value]${break value} else [error]${break 42}
d:=operation() else [error]{report(error)}
}
¬
main :: fn () {
    a := on yes => (optional() else {
        return 1
    })
        else 3
    b := on yes => identity(optional() else {
        return 2
    })
        else 3
    c := on operation() => [value] ${
        break value
    } else [error] ${
        break 42
    }
    d := operation() else [error] {
        report(error)
    }
}
