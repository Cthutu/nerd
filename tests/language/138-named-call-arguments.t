Counter :: plex {
    value i32
}

impl Counter {
    set :: fn (self: ^Self, amount: i32 = 1, scale: i32 = 2) {
        self.value = amount * scale
    }

    add :: fn (self: ^Self, amount: i32 = 1) {
        self.value += amount
    }

    get :: fn (self: Self) -> i32 {
        return self.value
    }
}

add :: fn (a: i32, b: i32 = 10, c: i32 = a + b) => a + b + c

main :: fn () {
    counter: Counter
    counter.set(amount = 7, scale = 3)
    counter.add(amount = 5)
    return add(a = 1, b = 2, c = 3) + counter.get()
}
¬
32
¬

¬
hir 0
bind Counter = type.0
bind __impl_4_set = fn.0
bind __impl_4_add = fn.1
bind __impl_4_get = fn.2
bind add = fn.3
bind main = fn.4
type type.0 = Counter
func fn.0(self: ^Counter, amount: i32, scale: i32) -> void {
  assign i32 field(^Counter local.0(self), value) = i32 multiply(i32 local.1(amount), i32 local.2(scale))
}
func fn.1(self: ^Counter, amount: i32) -> void {
  assign i32 field(^Counter local.3(self), value) = i32 add(i32 field(^Counter local.3(self), value), i32 local.4(amount))
}
func fn.2(self: Counter) -> i32 {
  return i32 field(Counter local.5(self), value)
}
func fn.3(a: i32, b: i32, c: i32) -> i32 {
  return i32 add(i32 add(i32 local.6(a), i32 local.7(b)), i32 local.8(c))
}
func fn.4() -> i32 {
  expr <unknown> <unsupported>
  let counter: Counter = <unknown> <unsupported>
  expr void call bind.1(__impl_4_set)(^Counter address_of(Counter local.9(counter)), i32 7, i32 3)
  expr void call bind.2(__impl_4_add)(^Counter address_of(Counter local.9(counter)), i32 5)
  return i32 add(i32 call bind.4(add)(i32 1, i32 2, i32 3), i32 call bind.3(__impl_4_get)(Counter local.9(counter)))
}
¬
; nerd llvm-ir 0
; generated from HIR

define void @fn.0(ptr %self, i32 %amount, i32 %scale) {
  %t0 = mul i32 %amount, %scale
  %t1 = getelementptr inbounds { i32 }, ptr %self, i64 0, i32 0
  store i32 %t0, ptr %t1
  ret void
}

define void @fn.1(ptr %self, i32 %amount) {
  %t0 = load { i32 }, ptr %self
  %t1 = extractvalue { i32 } %t0, 0
  %t2 = add i32 %t1, %amount
  %t3 = getelementptr inbounds { i32 }, ptr %self, i64 0, i32 0
  store i32 %t2, ptr %t3
  ret void
}

define i32 @fn.2({ i32 } %self) {
  %t0 = extractvalue { i32 } %self, 0
  ret i32 %t0
}

define i32 @fn.3(i32 %a, i32 %b, i32 %c) {
  %t0 = add i32 %a, %b
  %t1 = add i32 %t0, %c
  ret i32 %t1
}

define i32 @fn.4() {
  %local.9 = alloca { i32 }
  store { i32 } zeroinitializer, ptr %local.9
  call void @fn.0(ptr %local.9, i32 7, i32 3)
  call void @fn.1(ptr %local.9, i32 5)
  %t0 = add i32 1, 10
  %t1 = call i32 @fn.3(i32 1, i32 2, i32 3)
  %t2 = load { i32 }, ptr %local.9
  %t3 = call i32 @fn.2({ i32 } %t2)
  %t4 = add i32 %t1, %t3
  ret i32 %t4
}

@$__impl_4_set = alias void (ptr, i32, i32), ptr @fn.0
@$__impl_4_add = alias void (ptr, i32), ptr @fn.1
@$__impl_4_get = alias i32 ({ i32 }), ptr @fn.2
@$add = alias i32 (i32, i32, i32), ptr @fn.3
@$main = alias i32 (), ptr @fn.4
