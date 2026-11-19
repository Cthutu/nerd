// Hosted oracle for the initial-stack parser; native startup has its own probe.
#include "../../data/nrt.c"

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

static void check_parser(void)
{
    const char*        empty[]     = {NULL};
    const char*        values[]    = {"NO_COLORFUL=1",
                                      "TERMINFO=dumb",
                                      "NO_COLOR=",
                                      "TERM=xterm",
                                      "NO_COLOR=1",
                                      "TERM=dumb",
                                      NULL};
    const char*        plain[]     = {"NO_COLOR=1", "TERM=dumb", NULL};
    uintptr_t          auxiliary[] = {99, 77, 6, 4096, 0, 0};
    NrtProcessMetadata metadata;
    CHECK(nrt_read_process_metadata(empty, auxiliary, &metadata));
    CHECK(metadata.page_size == 4096 && !metadata.no_colour &&
          !metadata.dumb_terminal);
    CHECK(nrt_read_process_metadata(values, auxiliary, &metadata));
    CHECK(!metadata.no_colour &&
          !metadata.dumb_terminal); // first duplicate wins
    CHECK(nrt_read_process_metadata(plain, auxiliary, &metadata));
    CHECK(metadata.no_colour && metadata.dumb_terminal);
    for (size_t page = 4096; page <= 65536; page *= 2) {
        auxiliary[3] = page;
        CHECK(nrt_read_process_metadata(empty, auxiliary, &metadata));
        CHECK(metadata.page_size == page);
    }
    auxiliary[3] = 0;
    CHECK(!nrt_read_process_metadata(empty, auxiliary, &metadata));
    auxiliary[3] = 6000;
    CHECK(!nrt_read_process_metadata(empty, auxiliary, &metadata));
    uintptr_t missing[] = {99, 77, 0, 0};
    CHECK(!nrt_read_process_metadata(empty, missing, &metadata));
    uintptr_t duplicate[] = {6, 4096, 6, 65536, 0, 0};
    CHECK(!nrt_read_process_metadata(empty, duplicate, &metadata));
}

int main(void)
{
    check_parser();
    puts("runtime metadata passed");
    return 0;
}
