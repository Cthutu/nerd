State::enum {
    Idle


    Running(i32) -- active job identifier that is deliberately long enough to wrap onto another line while keeping the marker aligned

    Failed(string)
    -- keep this attached to Done without adding a gap
    Done
}
¬
State :: enum {
    Idle

    Running(i32) -- active job identifier that is deliberately long enough to
                 --     wrap onto another line while keeping the marker aligned

    Failed(string)
    -- keep this attached to Done without adding a gap
    Done
}
