pub Frame::plex {
    handle u64 -- native handle
    system ^FrameSystem -- owning system comment that is deliberately long
                        --     enough to wrap onto another line while keeping the
                        --     marker aligned
    mini ^FrameSystem -- short
                        --     more
    title string -- visible title
}
¬
pub Frame :: plex {
    handle u64          -- native handle
    system ^FrameSystem -- owning system comment that is deliberately long
                        --     enough to wrap onto another line while keeping the
                        --     marker aligned
    mini   ^FrameSystem -- short more
    title  string       -- visible title
}
