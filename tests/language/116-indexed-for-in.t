-- Iterates with both a usize index and an element binding.
Location :: plex {
    description string
    tag         string
}

locs: []Location = [
    { description : "d", tag : "a" },
]

main :: fn () -> i32 {
    noun := "a"
    for i, loc in locs {
        on noun == loc.tag => return i.as(i32)
    }
    return 0
}
¬
0
¬
¬
global locs
global locs$backing
fn main
local noun = string:"a"
local $0 = []plex{description:string,tag:string}:0
$0 = []plex{description:string,tag:string}:locs
local $1 = usize:0
$1 = usize:0
label L2
$5 = []plex{description:string,tag:string}:$0.count
$6 = usize:$1 < usize:$5
branch.false bool:$6, L4
local main$i$for42 = usize:$1
$7 = ^[]plex{description:string,tag:string}:$0[usize:$1]
local main$loc$for44 = ^plex{description:string,tag:string}:$7
block
$8 = ^plex{description:string,tag:string}:main$loc$for44.tag
$9 = string:noun == string:$8
branch.false bool:$9, L10
$11 = cast usize:main$i$for42
return i32:$11
label L10
end
label L3
$12 = usize:$1 + usize:1
$1 = usize:$12
jump L2
label L4
return i32:0
end
init
$0 = [1]plex{description:string,tag:string}:locs$backing[..]
locs = []plex{description:string,tag:string}:$0
$1 = plex(description: string:"d", tag: string:"a")
$2 = array[plex{description:string,tag:string}:$1]
locs$backing = [1]plex{description:string,tag:string}:$2
end
¬
