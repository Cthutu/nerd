PixelLayerMode :: enum {
    FixedSizeAutoScale {
        width u16
        height u16
    }
}

PixelLayer :: plex {
    mode PixelLayerMode
    width u16
    height u16
    data [..]u32
}

main :: fn () {
    layer := PixelLayer {
        mode: PixelLayerMode.FixedSizeAutoScale { width: 1 height: 1 }
        width: 1
        height: 1
        data: nil
    }
}
¬
PixelLayerMode :: enum {
    FixedSizeAutoScale {
        width  u16
        height u16
    }
}

PixelLayer :: plex {
    mode   PixelLayerMode
    width  u16
    height u16
    data   [..]u32
}

main :: fn () {
    layer := PixelLayer {
        mode   : PixelLayerMode.FixedSizeAutoScale { width: 1 height: 1 }
        width  : 1
        height : 1
        data   : nil
    }
}
