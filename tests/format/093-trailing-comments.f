use beta -- beta import
use alpha -- alpha import

pub Frame::plex {
    handle u64 -- native handle
    system ^FrameSystem -- owning system comment that is deliberately long enough to wrap onto another line while keeping the marker aligned
    title string -- visible title
}

main :: fn () {
    answer:=42 -- keep answer
    return -- return default
}

top::1 -- top comment
¬
use beta  -- beta import
use alpha  -- alpha import

pub Frame :: plex {
    handle u64           -- native handle
    system ^FrameSystem  -- owning system comment that is deliberately long
                         --     enough to wrap onto another line while keeping
                         --     the marker aligned
    title  string        -- visible title
}

main :: fn () {
    answer := 42  -- keep answer
    return  -- return default
}

top :: 1  -- top comment
