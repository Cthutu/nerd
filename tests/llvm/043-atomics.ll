use std.atomics

main :: fn () -> i32 {
    value : atomic[i32] = 1
    _ := value.load(Relaxed)
    _ := value.load(Acquire)
    _ := value.load(SequentiallyConsistent)
    value.store(2, Relaxed)
    value.store(2, Release)
    value.store(2, SequentiallyConsistent)
    _ := value.exchange(3, Relaxed)
    _ := value.exchange(3, Acquire)
    _ := value.exchange(3, Release)
    _ := value.exchange(3, AcquireRelease)
    _ := value.exchange(3, SequentiallyConsistent)
    _ := value.fetch_add(4, Relaxed)
    _ := value.fetch_sub(4, Acquire)
    _ := value.fetch_and(4, Release)
    _ := value.fetch_or(4, AcquireRelease)
    _ := value.fetch_xor(4, SequentiallyConsistent)
    _ := value.compare_exchange(7, 8, Relaxed, Relaxed)
    _ := value.compare_exchange(7, 8, Acquire, Acquire)
    _ := value.compare_exchange(7, 8, Release, Relaxed)
    _ := value.compare_exchange(7, 8, AcquireRelease, Acquire)
    result := value.compare_exchange_weak(7,
                                          8,
                                          SequentiallyConsistent,
                                          SequentiallyConsistent)
    on result {
        Exchanged => return 0
        else => return 1
    }
}
¬
%t0 = load atomic i32, ptr %local.0 monotonic, align 4
%t1 = load atomic i32, ptr %local.0 acquire, align 4
%t2 = load atomic i32, ptr %local.0 seq_cst, align 4
store atomic i32 2, ptr %local.0 monotonic, align 4
store atomic i32 2, ptr %local.0 release, align 4
store atomic i32 2, ptr %local.0 seq_cst, align 4
%t3 = atomicrmw xchg ptr %local.0, i32 3 monotonic, align 4
%t4 = atomicrmw xchg ptr %local.0, i32 3 acquire, align 4
%t5 = atomicrmw xchg ptr %local.0, i32 3 release, align 4
%t6 = atomicrmw xchg ptr %local.0, i32 3 acq_rel, align 4
%t7 = atomicrmw xchg ptr %local.0, i32 3 seq_cst, align 4
%t8 = atomicrmw add ptr %local.0, i32 4 monotonic, align 4
%t9 = atomicrmw sub ptr %local.0, i32 4 acquire, align 4
%t10 = atomicrmw and ptr %local.0, i32 4 release, align 4
%t11 = atomicrmw or ptr %local.0, i32 4 acq_rel, align 4
%t12 = atomicrmw xor ptr %local.0, i32 4 seq_cst, align 4
%t13 = cmpxchg ptr %local.0, i32 7, i32 8 monotonic monotonic, align 4
%t19 = cmpxchg ptr %local.0, i32 7, i32 8 acquire acquire, align 4
%t25 = cmpxchg ptr %local.0, i32 7, i32 8 release monotonic, align 4
%t31 = cmpxchg ptr %local.0, i32 7, i32 8 acq_rel acquire, align 4
%t37 = cmpxchg weak ptr %local.0, i32 7, i32 8 seq_cst seq_cst, align 4
