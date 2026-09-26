use std.atomics

main :: fn () -> i32 {
    value : atomic[i32] = 1
    _ := value.load(order = Relaxed)
    _ := value.load(order = Acquire)
    _ := value.load(order = SequentiallyConsistent)
    value.store(2, order = Relaxed)
    value.store(2, order = Release)
    value.store(2, order = SequentiallyConsistent)
    _ := value.exchange(3, order = Relaxed)
    _ := value.exchange(3, order = Acquire)
    _ := value.exchange(3, order = Release)
    _ := value.exchange(3, order = AcquireRelease)
    _ := value.exchange(3, order = SequentiallyConsistent)
    _ := value.fetch_add(4, order = Relaxed)
    _ := value.fetch_sub(4, order = Acquire)
    _ := value.fetch_and(4, order = Release)
    _ := value.fetch_or(4, order = AcquireRelease)
    _ := value.fetch_xor(4, order = SequentiallyConsistent)
    _ := value.compare_exchange(7, 8, success_order = Relaxed, failure_order = Relaxed)
    _ := value.compare_exchange(7, 8, success_order = Acquire, failure_order = Acquire)
    _ := value.compare_exchange(7, 8, success_order = Release, failure_order = Relaxed)
    _ := value.compare_exchange(7, 8, success_order = AcquireRelease, failure_order = Acquire)
    result := value.compare_exchange_weak(7,
                                          8,
                                          success_order = SequentiallyConsistent,
                                          failure_order = SequentiallyConsistent)
    on result {
        Exchanged => return 0
        else => return 1
    }
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [31 x i8] c"tests/llvm/043-atomics.input.n\00"

declare i32 @mN.fn.0(ptr %self, { i64, i8 } %order)
declare void @mN.fn.1(ptr %self, i32 %value, { i64, i8 } %order)
declare i32 @mN.fn.2(ptr %self, i32 %value, { i64, i8 } %order)
declare i32 @mN.fn.3(ptr %self, i32 %value, { i64, i8 } %order)
declare i32 @mN.fn.4(ptr %self, i32 %value, { i64, i8 } %order)
declare i32 @mN.fn.5(ptr %self, i32 %value, { i64, i8 } %order)
declare i32 @mN.fn.6(ptr %self, i32 %value, { i64, i8 } %order)
declare i32 @mN.fn.7(ptr %self, i32 %value, { i64, i8 } %order)
declare { i64, i32 } @mN.fn.8(ptr %self, i32 %expected, i32 %desired, { i64, i8 } %success_order, { i64, i8 } %failure_order)
declare { i64, i32 } @mN.fn.9(ptr %self, i32 %expected, i32 %desired, { i64, i8 } %success_order, { i64, i8 } %failure_order)

define internal i32 @fn.0() {
  %local.0 = alloca i32
  %local.14 = alloca { i64, i32 }
  %local.15 = alloca { i64, i32 }
  %local.16 = alloca { i64, i32 }
  %local.17 = alloca { i64, i32 }
  %local.18 = alloca { i64, i32 }
  store atomic i32 1, ptr %local.0 seq_cst, align 4
  %t0 = load atomic i32, ptr %local.0 seq_cst, align 4
  %t1 = load atomic i32, ptr %local.0 seq_cst, align 4
  %t2 = load atomic i32, ptr %local.0 seq_cst, align 4
  store atomic i32 2, ptr %local.0 seq_cst, align 4
  store atomic i32 2, ptr %local.0 seq_cst, align 4
  store atomic i32 2, ptr %local.0 seq_cst, align 4
  %t3 = atomicrmw xchg ptr %local.0, i32 3 seq_cst, align 4
  %t4 = atomicrmw xchg ptr %local.0, i32 3 seq_cst, align 4
  %t5 = atomicrmw xchg ptr %local.0, i32 3 seq_cst, align 4
  %t6 = atomicrmw xchg ptr %local.0, i32 3 seq_cst, align 4
  %t7 = atomicrmw xchg ptr %local.0, i32 3 seq_cst, align 4
  %t8 = atomicrmw add ptr %local.0, i32 4 seq_cst, align 4
  %t9 = atomicrmw sub ptr %local.0, i32 4 seq_cst, align 4
  %t10 = atomicrmw and ptr %local.0, i32 4 seq_cst, align 4
  %t11 = atomicrmw or ptr %local.0, i32 4 seq_cst, align 4
  %t12 = atomicrmw xor ptr %local.0, i32 4 seq_cst, align 4
  %t13 = cmpxchg ptr %local.0, i32 7, i32 8 seq_cst seq_cst, align 4
  %t14 = extractvalue { i32, i1 } %t13, 0
  %t15 = extractvalue { i32, i1 } %t13, 1
  %t16 = select i1 %t15, i64 0, i64 1
  %t17 = insertvalue { i64, i32 } poison, i64 %t16, 0
  %t18 = insertvalue { i64, i32 } %t17, i32 %t14, 1
  store { i64, i32 } %t18, ptr %local.14
  %t19 = cmpxchg ptr %local.0, i32 7, i32 8 seq_cst seq_cst, align 4
  %t20 = extractvalue { i32, i1 } %t19, 0
  %t21 = extractvalue { i32, i1 } %t19, 1
  %t22 = select i1 %t21, i64 0, i64 1
  %t23 = insertvalue { i64, i32 } poison, i64 %t22, 0
  %t24 = insertvalue { i64, i32 } %t23, i32 %t20, 1
  store { i64, i32 } %t24, ptr %local.15
  %t25 = cmpxchg ptr %local.0, i32 7, i32 8 seq_cst seq_cst, align 4
  %t26 = extractvalue { i32, i1 } %t25, 0
  %t27 = extractvalue { i32, i1 } %t25, 1
  %t28 = select i1 %t27, i64 0, i64 1
  %t29 = insertvalue { i64, i32 } poison, i64 %t28, 0
  %t30 = insertvalue { i64, i32 } %t29, i32 %t26, 1
  store { i64, i32 } %t30, ptr %local.16
  %t31 = cmpxchg ptr %local.0, i32 7, i32 8 seq_cst seq_cst, align 4
  %t32 = extractvalue { i32, i1 } %t31, 0
  %t33 = extractvalue { i32, i1 } %t31, 1
  %t34 = select i1 %t33, i64 0, i64 1
  %t35 = insertvalue { i64, i32 } poison, i64 %t34, 0
  %t36 = insertvalue { i64, i32 } %t35, i32 %t32, 1
  store { i64, i32 } %t36, ptr %local.17
  %t37 = cmpxchg weak ptr %local.0, i32 7, i32 8 seq_cst seq_cst, align 4
  %t38 = extractvalue { i32, i1 } %t37, 0
  %t39 = extractvalue { i32, i1 } %t37, 1
  %t40 = select i1 %t39, i64 0, i64 1
  %t41 = insertvalue { i64, i32 } poison, i64 %t40, 0
  %t42 = insertvalue { i64, i32 } %t41, i32 %t38, 1
  store { i64, i32 } %t42, ptr %local.18
  %t43 = load { i64, i32 }, ptr %local.18
  %t44 = insertvalue { i64, i32 } poison, i64 0, 0
  %t45 = insertvalue { i64, i32 } %t44, i32 0, 1
  %t46 = extractvalue { i64, i32 } %t43, 0
  %t47 = extractvalue { i64, i32 } %t45, 0
  %t48 = icmp eq i64 %t46, %t47
  br i1 %t48, label %on.body.1, label %on.next.2
on.body.1:
  ret i32 0
on.next.2:
  br label %on.body.3
on.body.3:
  ret i32 1
on.end.0:
  ret i32 0
}

@$main = hidden alias i32 (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)