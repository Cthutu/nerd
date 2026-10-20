main :: fn () { a := arena(4096)
 b := a
 b.done()
 a.done() }
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 2,
        "column": 7
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 7,
            "length": 1,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
¬
main :: fn () { a := arena(4096)
 b: arena
 b = a
 b.done()
 a.done() }
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 3,
        "column": 6
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 6,
            "length": 1,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
¬
take :: fn (a: arena) { a.reset() }
main :: fn () { a := arena(4096)
 take(a)
 a.done() }
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 3,
        "column": 7
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 7,
            "length": 1,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
¬
bad :: fn (a: ^arena) -> arena { return a^ }
main :: fn () { a := arena(4096)
 b := bad(^a)
 b.done()
 a.done() }
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 1,
        "column": 42
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 42,
            "length": 1,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
¬
Holder :: plex { storage arena }
main :: fn () { a := Holder { storage: arena(4096) }
 b := a
 b.storage.done()
 a.storage.done() }
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 3,
        "column": 7
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 7,
            "length": 1,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
¬
Holder :: plex { storage arena }
main :: fn () { a := Holder { storage: arena(4096) }
 b := a.storage
 b.done()
 a.storage.done() }
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 3,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 9,
            "length": 7,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
¬
main :: fn () { a := [arena(4096)]
 b := a
 b[0].done()
 a[0].done() }
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 2,
        "column": 7
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 7,
            "length": 1,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
¬
main :: fn () { a := (arena(4096),)
 b := a.0
 b.done()
 a.0.done() }
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 2,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 9,
            "length": 1,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
¬
Holder :: plex { storage arena
 n i32 }
main :: fn () { a := Holder { storage: arena(4096), n: 1 }
 b := a with { n: 2 }
 b.storage.done()
 a.storage.done() }
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 4,
        "column": 7
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 7,
            "length": 1,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
¬
duplicate :: fn [T](a: ^T) -> T { return a^ }
main :: fn () { a := arena(4096)
 b := duplicate[arena](^a)
 b.done()
 a.done() }
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 1,
        "column": 43
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 43,
            "length": 1,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
¬
Holder :: plex { storage arena }
impl Holder { count :: fn (self: Self) -> u32 { return self.storage.mark() } }
main :: fn () { a := Holder { storage: arena(4096) }
 assert a.count() == 0
 a.storage.done() }
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 4,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 9,
            "length": 1,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
¬
Holder :: plex { storage arena }
impl Holder { count :: fn (self: Self) -> u32 { return self.storage.mark() } }
main :: fn () { a := Holder { storage: arena(4096) }
 p := ^a
 assert p.count() == 0
 a.storage.done() }
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 5,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 9,
            "length": 1,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
¬
Holder :: enum { Empty Owned(arena) }
main :: fn () { a := Holder.Owned(arena(4096))
 on a { Empty => {} Owned(value) => value.reset() } }
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 3,
        "column": 27
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 27,
            "length": 5,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
¬
main :: fn () { a := [arena(4096)]
 b := a[0]
 b.done()
 a[0].done() }
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 2,
        "column": 8
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 8,
            "length": 1,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
¬
main :: fn () { a := arena(4096)
 b :: a
 b.reset()
 a.done() }
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 2,
        "column": 7
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 7,
            "length": 1,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
¬
main :: fn () { a := arena(4096)
 b := ${ break a }
 b.done()
 a.done() }
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 2,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 16,
            "length": 1,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
¬
Holder :: plex { storage arena }
main :: fn () { a := arena(4096)
 b := Holder { storage: a }
 b.storage.done()
 a.done() }
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 3,
        "column": 25
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 25,
            "length": 1,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
¬
main :: fn () { a := (arena(4096), 42)
 (b, _answer) := a
 b.done()
 a.0.done() }
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 2,
        "column": 18
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 18,
            "length": 1,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
¬
Holder :: enum { Empty Owned(arena) }
main :: fn () { a := Holder.Owned(arena(4096))
 b := a
 _b := b }
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 3,
        "column": 7
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 7,
            "length": 1,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}

¬
main :: fn () {
    storage :: arena(4096)
    pointer := ^storage
}
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 3,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 17,
            "length": 7,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
¬
Holder :: plex { storage arena }
main :: fn () {
    holder :: Holder { storage: arena(4096) }
    pointer := ^holder.storage
}
¬
{
    "message": "Arena storage cannot be copied",
    "source_file": "tests/errors/151-arena-copy.e",
    "primary_location": {
        "line": 4,
        "column": 24
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 24,
            "length": 7,
            "message": "This expression would copy an arena or an aggregate containing one"
        }
    ],
    "notes": [],
    "help": [
        "Use `^arena` to share an existing arena, or construct a fresh arena value."
    ]
}
