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
