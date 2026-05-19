Frame::plex {
    title string
    x i32
    y i32
    width u32
    height u32
}

frame:Frame:{
    title:"Example" -- title comment
    x:10 -- x comment

    -- size group
    width:640 -- width comment that should align with height and wrap onto the next line if the configured formatter width requires it
    height:480 -- height comment
}
¬
Frame :: plex {
    title  string
    x      i32
    y      i32
    width  u32
    height u32
}

frame : Frame : {
    title : "Example"  -- title comment
    x     : 10         -- x comment

    -- size group
    width  : 640  -- width comment that should align with height and wrap onto
                  --     the next line if the configured formatter width requires
                  --     it
    height : 480  -- height comment
}
