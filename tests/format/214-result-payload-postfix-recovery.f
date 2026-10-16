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
