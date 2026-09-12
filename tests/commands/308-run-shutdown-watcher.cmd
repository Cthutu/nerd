signal :: use std.signal
on "linux" {
    use os.linux
    send :: fn (number: i32) {
        pid := syscall(SYS_GETPID, 0, 0, 0, 0, 0, 0)
        assert syscall(SYS_KILL, pid.as(u64), number.as(u64), 0, 0, 0, 0) == 0
    }
    run :: fn () -> void\signal.Error {
        original: KernelSignalSet
        assert sys_rt_sigprocmask(SIG_BLOCK, nil, ^original) == 0
        watcher := signal.watch_shutdown()?
        defer _ := watcher.close()
        assert !(watcher.requested()?)
        on signal.watch_shutdown() => { assert no } else [error] {
            assert $"{error}" == "a shutdown watcher is already active"
        }
        send(SIGINT)
        watcher.wait()?
        assert watcher.requested()?
        watcher.wait()?
        send(SIGTERM)
        stale := watcher
        watcher.close()?
        watcher.close()?
        restored: KernelSignalSet
        assert sys_rt_sigprocmask(SIG_BLOCK, nil, ^restored) == 0
        assert restored == original
        on watcher.requested() => { assert no } else [error] {
            assert $"{error}" == "the shutdown watcher is closed"
        }
        next := signal.watch_shutdown()?
        defer _ := next.close()
        stale.close()?
        assert !(next.requested()?)
        send(SIGHUP)
        assert next.requested()?
        next.wait()?
    }
    main :: fn () { on run() => {} else { assert no } }
}
on !"linux" { main :: fn () {} }
¬
0
¬
