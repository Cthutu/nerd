FrameInfo :: plex {
    handle u64
}

FrameSystem :: plex {
    frames [..]FrameInfo
}

impl FrameSystem {
find_frame_info :: fn (fs: ^Self, handle: u64)
{
return for ^frame in fs.frames {
on frame.handle == handle => frame
} else {
nil
}
}
}
¬
FrameInfo :: plex {
    handle u64
}

FrameSystem :: plex {
    frames [..]FrameInfo
}

impl FrameSystem {

    find_frame_info :: fn (fs: ^Self, handle: u64) {
        return for ^frame in fs.frames {
            on frame.handle == handle => frame
        } else {
            nil
        }
    }

}
