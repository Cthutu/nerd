// Work estimates for opt-in automatic dispatch. Numeric jobs bypass this
// policy.
#pragma once
#include <compiler/internal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    usize count;
    u64   total;
    u64   largest;
} CompilerTaskWork;

static inline void compiler_task_work_add(CompilerTaskWork* work, u64 weight)
{
    work->count++;
    work->total =
        weight > U64_MAX - work->total ? U64_MAX : work->total + weight;
    if (weight > work->largest) {
        work->largest = weight;
    }
}

// Initial calibration candidates, to be measured before adoption.
#define COMPILER_PARSE_GRAIN 65536
#define COMPILER_HIR_GRAIN 8192
#define COMPILER_RENDER_GRAIN 512

static inline u32
compiler_task_jobs(u32 ceiling, CompilerTaskWork work, u64 grain)
{
    return task_jobs_for_work(
        ceiling, work.count, work.total, work.largest, grain);
}

static inline void compiler_task_policy_emit(
    cstr phase, u32 ceiling, u32 jobs, CompilerTaskWork work, u64 grain)
{
    const char* profile = getenv("NERD_PROFILE");
    if (profile != NULL && strcmp(profile, "1") == 0) {
        fprintf(stderr,
                "nerd-profile\t{\"kind\":\"scheduler-policy\",\"phase\":\"%s\","
                "\"ceiling\":%u,\"jobs\":%u,\"modules\":%zu,\"work\":%llu,"
                "\"largest\":%llu,\"grain\":%llu}\n",
                phase, ceiling, jobs, work.count,
                (unsigned long long)work.total,
                (unsigned long long)work.largest, (unsigned long long)grain);
    }
}
