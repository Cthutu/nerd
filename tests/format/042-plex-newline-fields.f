use std.print

Person :: plex { name string age u8 }

matt :: Person {
name: "Matt"
          age: 53
}

main :: fn () {
    prn($"His name is {matt.name} and he is {matt.age} years old")
}
¬
use std.print

Person :: plex {
    name string
    age  u8
}

matt :: Person {
    name: "Matt"
    age : 53
}

main :: fn () {
    prn($"His name is {matt.name} and he is {matt.age} years old")
}
