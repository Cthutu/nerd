main :: fn () {
    mobs := [
        { id: 1 name: "bear" plural: "bears" },
        {
            id: 3
            name: "gnoll"
            plural: "gnolls"
        },
        { id: 7 name: "wolf" plural: "wolves" },
    ]
}
¬
main :: fn () {
    mobs := [
        { id : 1 name : "bear"  plural : "bears" },
        { id : 3 name : "gnoll" plural : "gnolls" },
        { id : 7 name : "wolf"  plural : "wolves" },
    ]
}
¬
main :: fn () {
    value := Outer {
        child: Inner {
            x: 1
        }
        description: "A deliberately long description that keeps the outer literal multiline."
    }
}
¬
main :: fn () {
    value := Outer {
        child       : Inner { x: 1 }
        description : "A deliberately long description that keeps the outer literal multiline."
    }
}
¬
make :: fn () => Point {
    x: 1
    y: 2
}
main :: fn () {
    item := Point {
        x: 1 -- Keep the comment.
        y: 2
    }
    grouped := Point {
        x: 1

        y: 2
    }
}
¬
make :: fn () => Point { x: 1 y: 2 }

main :: fn () {
    item := Point {
        x : 1  -- Keep the comment.
        y : 2
    }
    grouped := Point {
        x: 1

        y: 2
    }
}
¬
main :: fn () {
    vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv := Point {
        x: 1
    }
}
¬
main :: fn () {
    vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv := Point { x: 1 }
}
¬
main :: fn () {
    vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv := Point {
        x: 1
    }
}
¬
main :: fn () {
    vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv := Point {
        x: 1
    }
}
¬
main :: fn () {
    text_width := measure(text)
    clip := prepare(Rect {
        x
        y
        w: text_width
        h: 1
    })
    on pressed {
        yes => events.push(Event.Press {
            scan_code
        })
        no => events.push(Event.Release {
            scan_code
        })
    }
    term_fb_fill_rect(0,
                      0,
                      width,
                      height,
                      ch = ' ',
                      ink = text_ink,
                      paper = rgb(0, 0, 0))
}
¬
main :: fn () {
    text_width := measure(text)
    clip       := prepare(Rect { x y w: text_width h: 1 })

    on pressed {
        yes  => events.push(Event.Press { scan_code })
        no   => events.push(Event.Release { scan_code })
    }
    term_fb_fill_rect(0,
                      0,
                      width,
                      height,
                      ch = ' ',
                      ink = text_ink,
                      paper = rgb(0, 0, 0))
}
