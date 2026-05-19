pub Frame::plex {
    -- first field lead
    handle u64 -- native handle
    -- standalone after trailing
    system ^FrameSystem -- owning system comment
                        --     wrapped continuation
    -- standalone before title
    title string -- visible title
    -- final standalone
}
¬
pub Frame :: plex {
    -- first field lead
    handle u64           -- native handle
    -- standalone after trailing
    system ^FrameSystem  -- owning system comment wrapped continuation
    -- standalone before title
    title  string        -- visible title
    -- final standalone
}
