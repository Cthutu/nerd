Room :: plex {
    name        string
    description string
    exits       [4]i32
}

rooms : []Room : [
    {
        name        : "Hall"
        description : "A dusty entrance hall."
        exits       : [0, 1, 2, 3]
    },
    {
        name        : "Kitchen"
        description : "An old kitchen."
        exits       : [0, 0, 0, 1]
    },
]

items : []Room : [
    { name : "key",     description : "A small brass key.",      exits : [1, 2, 3, 4] },
    { name : "lantern", description : "A battered oil lantern.", exits : [5, 6, 7, 8] },
    { name : "coin",    description : "An old silver coin.",     exits : [9, 10, 11, 12] },
]
¬
Room :: plex {
    name        string
    description string
    exits       [4]i32
}

rooms : []Room : [
    {
        name:        "Hall"
        description: "A dusty entrance hall."
        exits:       [0, 1, 2, 3]
    },
    {
        name:        "Kitchen"
        description: "An old kitchen."
        exits:       [0, 0, 0, 1]
    },
]

items : []Room : [
    { name: "key",     description: "A small brass key.",      exits: [1, 2, 3, 4] },
    { name: "lantern", description: "A battered oil lantern.", exits: [5, 6, 7, 8] },
    { name: "coin",    description: "An old silver coin.",     exits: [9, 10, 11, 12] },
]
