// Linux syscall I/O exercised from a hosted process, including worker failures.
#include "../../data/nrt.c"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <sys/prctl.h>
#include <sys/time.h>

#define CHECK(condition)                                                       \
    do {                                                                       \
        if (!(condition)) {                                                    \
            fprintf(stderr,                                                    \
                    "check failed at line %d: %s\n",                           \
                    __LINE__,                                                  \
                    #condition);                                               \
            exit(1);                                                           \
        }                                                                      \
    } while (0)

static volatile sig_atomic_t alarms;

static void alarm_handler(int number)
{
    (void)number;
    ++alarms;
}

static void start_interrupts(void)
{
    struct sigaction action = {0};
    action.sa_handler       = alarm_handler;
    CHECK(sigemptyset(&action.sa_mask) == 0);
    // Deliberately omit SA_RESTART to exercise EINTR/partial syscall results.
    CHECK(sigaction(SIGALRM, &action, NULL) == 0);
    struct itimerval timer = {.it_interval = {.tv_usec = 1000},
                              .it_value    = {.tv_usec = 1000}};
    CHECK(setitimer(ITIMER_REAL, &timer, NULL) == 0);
}

static void foreign_done(void) { nrt_eprn("foreign atexit", 14); }

static void* failing_worker(void* context)
{
    (void)context;
    nrt_abort("worker failure", 14);
    return NULL;
}

static void returning_abort_handler(int number)
{
    (void)number;
    nrt_eprn("abort handler", 13);
}

static void exiting_abort_handler(int number)
{
    (void)number;
    _Exit(42);
}

static void* aborting_worker(void* context)
{
    (void)context;
    nrt_signal_abort();
}

static void check_diagnostics(void)
{
    char text[1025];
    for (size_t i = 0; i < sizeof(text) - 1; ++i) {
        text[i] = (char)('a' + i % 26);
    }
    text[sizeof(text) - 1] = 0;
    char        expected[2048];
    const char* format =
        "%% [%s] [%u] [%llu] [%zu] [%-5s] [%*s] [%*llu] [%*zu] [%*s]\n%s\n";
    int count = snprintf(expected,
                         sizeof(expected),
                         format,
                         "string",
                         UINT_MAX,
                         ULLONG_MAX,
                         SIZE_MAX,
                         "heap",
                         9,
                         "index",
                         23,
                         ULLONG_MAX,
                         24,
                         SIZE_MAX,
                         -7,
                         "left",
                         text);
    CHECK(count > 0 && (size_t)count < sizeof(expected));
    nrt_pr(expected, (size_t)count);
    nrt_eprintf(format,
                "string",
                UINT_MAX,
                ULLONG_MAX,
                SIZE_MAX,
                "heap",
                9,
                "index",
                23,
                ULLONG_MAX,
                24,
                SIZE_MAX,
                -7,
                "left",
                text);
    nrt_eprintf("%s", (const char*)NULL);
    nrt_pr("(null)", 6);
    nrt_eprintfn("%u %llu %zu", 0u, 0ull, (size_t)0);
    nrt_pr("0 0 0\n", 6);
}

int main(int argc, char** argv)
{
#if defined(NRT_LINUX_EXECUTABLE)
    // Hosted oracle: initialise explicit metadata without any CRT-specific
    // constructor argument convention. The native bootstrap is tested
    // separately.
    uintptr_t auxiliary[] = {6, (uintptr_t)sysconf(_SC_PAGESIZE), 0, 0};
    CHECK(nrt_read_process_metadata(
        (const char* const*)environ, auxiliary, &g_nrt_process_metadata));
#endif
    CHECK(argc == 2);
    // Observe signal termination without producing core dumps in the test gate.
    CHECK(prctl(PR_SET_DUMPABLE, 0) == 0);
    if (strncmp(argv[1], "signal-", 7) == 0) {
        CHECK(atexit(foreign_done) == 0);
        struct sigaction action = {0};
        CHECK(sigemptyset(&action.sa_mask) == 0);
        action.sa_handler = SIG_DFL;
        if (strcmp(argv[1], "signal-ignore") == 0) {
            action.sa_handler = SIG_IGN;
        }
        if (strcmp(argv[1], "signal-handler") == 0) {
            action.sa_handler = returning_abort_handler;
        }
        if (strcmp(argv[1], "signal-handler-exit") == 0) {
            action.sa_handler = exiting_abort_handler;
        }
        CHECK(sigaction(SIGABRT, &action, NULL) == 0);
        if (strcmp(argv[1], "signal-blocked") == 0) {
            sigset_t mask;
            CHECK(sigemptyset(&mask) == 0);
            CHECK(sigaddset(&mask, SIGABRT) == 0);
            CHECK(pthread_sigmask(SIG_BLOCK, &mask, NULL) == 0);
        }
        if (strcmp(argv[1], "signal-worker") == 0) {
            pthread_t worker;
            CHECK(pthread_create(&worker, NULL, aborting_worker, NULL) == 0);
            CHECK(pthread_join(worker, NULL) == 0);
            CHECK(false);
        }
        nrt_signal_abort();
    } else if (strcmp(argv[1], "stack-fail") == 0) {
        CHECK(atexit(foreign_done) == 0);
        __stack_chk_fail();
    } else if (strcmp(argv[1], "slice-abort") == 0) {
        NerdString result;
        nrt_string_slice(&result, NULL, 0, 1);
        CHECK(false);
    } else if (strcmp(argv[1], "output") == 0) {
        nrt_pr(NULL, 999);
        nrt_epr(NULL, 999);
        nrt_pr("A\0B", 3);
        nrt_prn(NULL, 999);
        nrt_prn("tail", 4);
        nrt_epr("E\0F", 3);
        nrt_eprn(NULL, 0);
        nrt_eprn("error", 5);
    } else if (strcmp(argv[1], "input") == 0) {
        for (int i = 0; i < 8; ++i) {
            u8     buffer[3];
            size_t count = nrt_read_line(buffer, sizeof(buffer));
            nrt_prn(buffer, count);
        }
    } else if (strcmp(argv[1], "empty-view") == 0) {
        CHECK(nrt_read_line(NULL, 0) == 0);
        u8     buffer[4];
        size_t count = nrt_read_line(buffer, sizeof(buffer));
        nrt_prn(buffer, count);
    } else if (strcmp(argv[1], "diagnostics") == 0) {
        check_diagnostics();
    } else if (strcmp(argv[1], "invalid-format") == 0) {
        nrt_eprintf("%d", 1);
    } else if (strcmp(argv[1], "bad-output") == 0) {
        CHECK(close(1) == 0);
        nrt_pr("lost", 4);
        nrt_eprn("survived", 8);
    } else if (strcmp(argv[1], "nonblocking-output") == 0) {
        int descriptors[2];
        CHECK(pipe2(descriptors, O_NONBLOCK) == 0);
        u8 bytes[4096] = {0};
        while (write(descriptors[1], bytes, sizeof(bytes)) > 0) {
        }
        CHECK(errno == EAGAIN);
        CHECK(dup2(descriptors[1], 1) == 1);
        nrt_pr("full pipe", 9);
        CHECK(signal(SIGPIPE, SIG_IGN) != SIG_ERR);
        CHECK(close(descriptors[0]) == 0);
        nrt_pr("broken pipe", 11);
        CHECK(close(descriptors[1]) == 0);
        nrt_eprn("survived", 8);
    } else if (strcmp(argv[1], "broken-output") == 0) {
        int descriptors[2];
        CHECK(pipe(descriptors) == 0);
        CHECK(dup2(descriptors[1], 1) == 1);
        CHECK(close(descriptors[0]) == 0);
        CHECK(signal(SIGPIPE, SIG_DFL) != SIG_ERR);
        nrt_pr("broken pipe", 11);
        CHECK(false);
    } else if (strcmp(argv[1], "bad-input") == 0) {
        CHECK(close(0) == 0);
        u8 bytes[3];
        CHECK(nrt_read_line(bytes, sizeof(bytes)) == 0);
    } else if (strcmp(argv[1], "nonblocking-input") == 0) {
        int descriptors[2];
        CHECK(pipe2(descriptors, O_NONBLOCK) == 0);
        CHECK(dup2(descriptors[0], 0) == 0);
        u8 bytes[4];
        CHECK(nrt_read_line(bytes, sizeof(bytes)) == 0);
        CHECK(write(descriptors[1], "next\n", 5) == 5);
        CHECK(nrt_read_line(bytes, sizeof(bytes)) == 4);
        nrt_prn(bytes, sizeof(bytes));
        CHECK(close(descriptors[0]) == 0);
        CHECK(close(descriptors[1]) == 0);
    } else if (strcmp(argv[1], "mixed-stdio") == 0) {
        CHECK(fputs("foreign ", stdout) >= 0);
        CHECK(fflush(stdout) == 0);
        nrt_prn("runtime", 7);
    } else if (strcmp(argv[1], "write-interrupt") == 0) {
        static u8 bytes[1024 * 1024];
        for (size_t i = 0; i < sizeof(bytes); ++i) {
            bytes[i] = (u8)(i % 251);
        }
        start_interrupts();
        nrt_pr(bytes, sizeof(bytes));
        CHECK(alarms > 0);
    } else if (strcmp(argv[1], "read-interrupt") == 0) {
        start_interrupts();
        nrt_eprn("ready", 5);
        u8     bytes[8];
        size_t count = nrt_read_line(bytes, sizeof(bytes));
        CHECK(alarms > 0);
        nrt_prn(bytes, count);
    } else if (strcmp(argv[1], "report") == 0) {
        nrt_prn("before", 6);
        NerdString message = {(u8*)"problem", 7};
        nrt_report_main_error(&message);
    } else if (strcmp(argv[1], "fatal") == 0) {
        CHECK(atexit(foreign_done) == 0);
        nrt_prn("before failure", 14);
        nrt_abort("fatal failure", 13);
    } else if (strcmp(argv[1], "assert") == 0) {
        NerdString message = {(u8*)"binary\0message", 14};
        nrt_nerd_assert(false, "probe.n", 42, &message);
    } else if (strcmp(argv[1], "worker-fatal") == 0) {
        CHECK(atexit(foreign_done) == 0);
        pthread_t worker;
        CHECK(pthread_create(&worker, NULL, failing_worker, NULL) == 0);
        // exit_group must terminate this waiting thread as well as the worker.
        CHECK(pthread_join(worker, NULL) == 0);
        CHECK(false);
    } else if (strcmp(argv[1], "normal-return") == 0) {
        CHECK(atexit(foreign_done) == 0);
        nrt_prn("normal", 6);
    } else {
        CHECK(false);
    }
    return 0;
}
