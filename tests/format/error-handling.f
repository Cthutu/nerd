main::fn(){total:=0 defer total+=1 defer{total+=2

total+=3}return total}
¬
main :: fn () {
    total := 0
    defer total += 1
    defer {
        total += 2

        total += 3
    }
    return total
}
¬
main::fn(){defer _:=close_file()}
¬
main :: fn () {
    defer _ := close_file()
}
¬
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
¬
acquire :: fn () -> ?void {}
main :: fn () -> ?void { acquire()? undo prn("first")
undo { prn("second") }
return }

¬
acquire :: fn () -> ?void {
}

main :: fn () -> ?void {
    acquire()?
    undo prn("first")
    undo {
        prn("second")
    }
    return
}
¬
main :: fn () { defer assert yes
undo assert no,"cleanup failed"
defer assert 1 == 1,"complete"
}

¬
main :: fn () {
    defer assert yes
    undo assert no, "cleanup failed"
    defer assert 1 == 1, "complete"
}
¬
run :: fn (enabled: bool) -> void\Error {
    on enabled => return Error.Timeout { code: 0 }!
    on operation() {
        value => { _ := value }
        error! => return error!
    }
    broken(&value)
}
¬
run :: fn (enabled: bool) -> void\Error {
    on enabled => return Error.Timeout {
        code: 0
    }!
    on operation() {
        value => {
            _ := value
        }
        error! => return error!
    }
    broken(& value)
}
¬
pump :: fn () -> void\Error {
    entries := [PollEntry {
        -- Wait until the stream accepts another write.
        socket: stream write: yes ...
    }]
    on operation() {
        count => { _ := count }
        Error.WouldBlock { code }! => { _ := code }
        error! => return error!
    }
    on other() {
        value => { _ := value }
        _! => return Error.Timeout { code: 0 }!
    }
}
¬
pump :: fn () -> void\Error {
    entries := [
        PollEntry {
            -- Wait until the stream accepts another write.
            socket : stream
            write  : yes
            ...
        },
    ]
    on operation() {
        count => {
            _ := count
        }

        Error.WouldBlock { code }! => {
            _ := code
        }

        error! => return error!
    }
    on other() {
        value => {
            _ := value
        }

        _! => return Error.Timeout { code: 0 }!
    }
}
¬
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
¬
run :: fn () -> void \ Error {
    value := operation()\fallback()\42
    result : i32 \ Error = operation()\42
    broken(&value)
    later : ?i32 \ Error = operation()\42
    grouped := operation()\(42)
    array := operation()\[42]
}
¬
run :: fn () -> void\Error {
    value := operation() \ fallback() \ 42
    result : i32\Error = operation() \ 42
    broken(& value)
    later : ?i32\Error = operation() \ 42
    grouped := operation() \ (42)
    array := operation() \ [42]
}
¬
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
¬
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
¬
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
