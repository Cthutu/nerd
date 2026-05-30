use std.io

Mode :: enum {
    Fixed {
        width u16
        height u16
    }
}

Layer :: plex {
    mode Mode
    width u16
    height u16
}

main :: fn () {
    layer := Layer {
        mode   : Mode.Fixed { width: 1 height: 2 }
        width  : 3
        height : 4
    }

    prn($"{layer.width}x{layer.height}")
}
¬
0
¬
3x4

¬
