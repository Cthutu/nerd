Layer :: plex {
    width u16
    data [..]u32
}

Handle :: plex {
    index usize
}

System :: plex {
    layers [..]Layer
}

resize :: fn (layer: ^Layer, width: u16) {
    layer.width = width
    layer.data.resize_undefined_to(width.as(usize))
}

impl Layer {

    pixel_width :: fn (layer: ^Self) -> u16 {
        return layer.width
    }

}

impl System {

    init :: fn () -> Self {
        return { layers: nil }
    }

    done :: fn (system: ^Self) {
        system.layers[0].data.free()
        system.layers.free()
    }

    add :: fn (system: ^Self, width: u16) -> Handle {
        layer := Layer { width: 0 data: nil }
        resize(^layer, width)
        system.layers.push(layer)
        return Handle { index: system.layers.count - 1 }
    }

    get :: fn (system: ^Self, handle: Handle) -> ^Layer {
        on handle.index >= system.layers.count => return nil
        return ^system.layers[handle.index]
    }

}

main :: fn () {
    system := System.init()
    defer system.done()

    handle := system.add(4)
    layer := system.get(handle)

    assert layer != nil
    assert layer.pixel_width() == 4
}
¬
0
¬

¬
