# Imported generic specialization: existing incorrect return

Found while developing M5 concurrency fixtures on 2026-09-20. Reproduced with
both the pre-M5 release compiler (`0a5b6648`) and M5 (`81a566dd`), with `--jobs 1`.
Both compile successfully and the executable exits 249 (the low byte of -7),
where the source should return zero. Fixed during M8 on 2026-09-21: HIR discarded
the selected specialization symbol, and LLVM resolution compared module-local
type indices across different modules. HIR now preserves the symbol and LLVM
resolves it against the imported module's specialization symbols. The previous
failed resolution stopped body emission, triggering its default zero return.

Place these four files together, then run `nerd build --jobs 1 main.n -o repro`
and `./repro`. The imported `right.value` LLVM body was `ret i32 0`, losing its
explicit `return 7`, after the two modules instantiated the shared generic with
different integer types. The scheduling suite now uses this distinct-type reproduction and also checks
used return values, inferred calls and first-class specialized functions. Every
worker count executes the result; generated C is compiled externally and executed
too. The compiler itself does not invoke Clang.

## main.n

```nerd
left :: use left
right :: use right
duplicate :: use left
main :: fn () -> i32 { return left.value() + right.value() + duplicate.value() - 21 }
```

## leaf.n

```nerd
pub id :: fn [T] (value: T) -> T { return value }
pub ffi "c" { absolute :: abs (value: i32) -> i32 }
```

## left.n

```nerd
use leaf
pub value :: fn () -> i32 { return absolute(id[i32](-7)) }
```

## right.n

```nerd
use leaf
pub value :: fn () -> i32 { _x := id[i64](7)
 return 7 }
```
