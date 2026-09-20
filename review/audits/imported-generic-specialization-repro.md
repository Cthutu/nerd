# Imported generic specialization: existing incorrect return

Found while developing M5 concurrency fixtures on 2026-09-20. Reproduced with
both the pre-M5 release compiler (`0a5b6648`) and M5 (`81a566dd`), with `--jobs 1`.
Both compile successfully and the executable exits 249 (the low byte of -7),
where the source should return zero. This remains an open code-generation issue;
its root cause has not been established.

Place these four files together, then run `nerd build --jobs 1 main.n -o repro`
and `./repro`. The imported `right.value` LLVM body was `ret i32 0`, losing its
explicit `return 7`, after the two modules instantiated the shared generic with
different integer types. The same-type shared-instantiation fixture used by the
M5 suite returns zero correctly. This reproduction is tracked separately from
the scheduling parity gate rather than making a regression test assert the
incorrect exit value.

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
