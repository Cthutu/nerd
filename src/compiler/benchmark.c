//------------------------------------------------------------------------------
// Compiler benchmark and phase execution helpers
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/internal.h>

//------------------------------------------------------------------------------

bool compiler_phase_run(const PhaseSpec* phases,
                        usize            phase_count,
                        void*            context,
                        Timing*          timing)
{
    for (usize i = 0; i < phase_count; i++) {
        const PhaseSpec* phase  = &phases[i];
        bool             result = true;
        if (timing != NULL) {
            ThreadTimePoint start = thread_time_now();
            result                = phase->run(context);
            ThreadTimePoint end   = thread_time_now();
            timing_add(timing,
                       phase->stage,
                       phase->phase,
                       thread_time_elapsed(start, end));
        } else {
            result = phase->run(context);
        }

        if (!result) {
            return false;
        }
    }
    return true;
}

void compiler_phase_reset_reverse(const PhaseSpec* phases,
                                  usize            phase_count,
                                  void*            context)
{
    for (usize i = phase_count; i > 0; i--) {
        phases[i - 1].reset(context);
    }
}

TimeDuration compiler_phase_benchmark_single(const PhaseSpec* phases,
                                             usize            phase_count,
                                             usize            phase_index,
                                             void*            context,
                                             u32              warmup_iterations,
                                             u32              timed_iterations)
{
    ASSERT(phase_index < phase_count, "Phase index out of range");
    if (timed_iterations == 0) {
        return 0;
    }

    const PhaseSpec* phase = &phases[phase_index];

    for (usize i = 0; i < phase_index; i++) {
        if (!phases[i].run(context)) {
            return 0;
        }
    }

    for (u32 i = 0; i < warmup_iterations; i++) {
        if (!phase->run(context)) {
            return 0;
        }
        phase->reset(context);
    }

    ThreadTimePoint start = thread_time_now();
    for (u32 i = 0; i < timed_iterations; i++) {
        if (!phase->run(context)) {
            return 0;
        }
        phase->reset(context);
    }
    ThreadTimePoint end = thread_time_now();

    for (usize i = phase_index; i > 0; i--) {
        phases[i - 1].reset(context);
    }

    TimeDuration total = thread_time_elapsed(start, end);
    TimeDuration avg   = (total + (TimeDuration)(timed_iterations / 2)) /
                       (TimeDuration)timed_iterations;
    if (avg == 0 && total > 0) {
        // Preserve sub-tick non-zero work instead of truncating to zero.
        avg = 1;
    }
    return avg;
}

//------------------------------------------------------------------------------
