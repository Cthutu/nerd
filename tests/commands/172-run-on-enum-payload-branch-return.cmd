use std.io

Size :: plex {
    width u16
    height u16
}

Mode :: enum {
    Fit {
        scale u16
    }

    Fixed {
        width u16
        height u16
    }
}

dims :: fn (mode: Mode) -> Size {
    on mode {
        Fit { scale } => {
            return Size { width: scale height: scale }
        }
        Fixed { width, height } => {
            return Size { width: width height: height }
        }
    }

    return Size { width: 1 height: 1 }
}

main :: fn () {
    size := dims(Mode.Fixed { width: 10 height: 20 })
    prn($"{size.width}x{size.height}")
}
¬
0
¬
10x20

¬
