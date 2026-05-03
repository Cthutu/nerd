use std.io

Person :: plex { name string age u8 }

matt :: Person {
name: "Matt"
          age: 53
}

main :: fn () {
    prn($"His name is {matt.name} and he is {matt.age} years old")
}
¬
0
¬
His name is Matt and he is 53 years old

¬
global matt
fn main
string.reset
$0 = string.start
string.append string:"His name is "
$2 = plex{name:string,age:u8}:matt.name
string.append string:$2
string.append string:" and he is "
$3 = plex{name:string,age:u8}:matt.age
string.append u8:$3
string.append string:" years old"
$1 = string.finish $0
call fn(string)->void:prn, string:$1
string.reset
return i32:0
end
init
$0 = plex(name: string:"Matt", age: u8:53)
matt = plex{name:string,age:u8}:$0
end
¬
#ifndef NERD_TYPE_plex008635ad
#define NERD_TYPE_plex008635ad
typedef struct plex008635ad {
    string $name;
    uint8_t $age;
} plex008635ad;
#endif
plex008635ad $matt;
int $main() {
    string_builder_reset();
    size_t $0 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"His name is ", .count = 12}));
    string $2 = $matt.$name;
    string_builder_append_string(to_string$string($2));
    string_builder_append_string(to_string$string((string){.data = (u8*)" and he is ", .count = 11}));
    uint8_t $3 = $matt.$age;
    string_builder_append_string(to_string$u8($3));
    string_builder_append_string(to_string$string((string){.data = (u8*)" years old", .count = 10}));
    string $1 = string_builder_finish($0);
    prn($1);
    string_builder_reset();
    return 0;
}
void init() {
    plex008635ad $0 = (plex008635ad){.$name = (string){.data = (u8*)"Matt", .count = 4}, .$age = 53};
    $matt = $0;
}
