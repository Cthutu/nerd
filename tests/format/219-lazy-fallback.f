main::fn()->void \ Error{
a:=first()\second()\42
b:=first()\2*3+1
c:=first()\(2*3+1)
d:=(first()\second())\42
e:=first()\-2
f:=first()\${break 42}
g:=first()\second().field
}
¬
main :: fn () -> void\Error {
    a := first() \ second() \ 42
    b := first() \ 2 * 3 + 1
    c := first() \ (2 * 3 + 1)
    d := (first() \ second()) \ 42
    e := first() \ -2

    f := first() \ ${
        break 42
    }
    g := first() \ second().field
}
