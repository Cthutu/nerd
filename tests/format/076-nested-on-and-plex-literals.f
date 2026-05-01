main::fn(){on value==2=>{on{value==1=>hit=1 else=>hit=2}}}

objs: []Object = [
    { description: "a silver coin", tag: "silver", location: ^locs[0] },
    { description: "a gold coint", tag: "gold", location: ^locs[1] },
    { description: "a burly guard", tag: "guard", location: ^locs[0] },
]
¬
main :: fn () {
    on value == 2 => {
        on {
            value == 1 => hit = 1
            else => hit = 2
        }
    }
}

objs: []Object = [
    { description: "a silver coin", tag: "silver", location: ^locs[0] },
    { description: "a gold coint",  tag: "gold",   location: ^locs[1] },
    { description: "a burly guard", tag: "guard",  location: ^locs[0] },
]
