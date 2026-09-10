Display :: trait{show::fn(self: Self)->string}
Hasher :: trait {
hash :: fn(self: Self)->u64
finish :: fn()->u64
}
DisplayValue :: trait for Value{show::fn(self: Value)->string}
¬
Display :: trait {
    show :: fn (self: Self) -> string
}

Hasher :: trait {
    hash   :: fn (self: Self) -> u64
    finish :: fn () -> u64
}

DisplayValue :: trait for Value {
    show :: fn (self: Value) -> string
}
