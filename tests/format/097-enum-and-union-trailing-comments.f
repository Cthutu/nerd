State::enum {
    Idle -- no work pending
    Running(i32) -- active job identifier that is deliberately long enough to wrap onto another line while keeping the marker aligned
    Failed(string, i32) -- failure payload
    -- standalone after trailing
    Done = 4 -- explicit discriminant
}

Value::union {
    i i32 -- integer value
    text string -- text value
}
¬
State :: enum {
    Idle                 -- no work pending
    Running(i32)
                         -- active job identifier that is deliberately long
                         --     enough to wrap onto another line while keeping
                         --     the marker aligned
    Failed(string, i32)  -- failure payload
    -- standalone after trailing
    Done = 4  -- explicit discriminant
}

Value :: union {
    i    i32     -- integer value
    text string  -- text value
}
