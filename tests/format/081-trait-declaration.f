Display :: trait{show::fn(Self)->string}
Hasher :: trait {
hash :: fn(Self)->u64
finish :: fn()->u64
}
DisplayValue :: trait for Value{show::fn(Value)->string}
¬
Display :: trait {
    show :: fn (Self) -> string
}

Hasher :: trait {
    hash   :: fn (Self) -> u64
    finish :: fn () -> u64
}

DisplayValue :: trait for Value {
    show :: fn (Value) -> string
}
