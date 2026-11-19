// Compile with NRT_LINUX_EXECUTABLE and link the environment-mutating DSO.
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

extern int  metadata_provider(void);
static bool constructor_ran;

__attribute__((constructor)) static void check_early_metadata(void)
{
    CHECK(nrt_page_size() == (size_t)sysconf(_SC_PAGESIZE));
    // The provider has already changed the live libc environment. The captured
    // flags and auxv must nevertheless come from before that constructor.
    CHECK(strcmp(getenv("NO_COLOR"), "1") == 0);
    CHECK(strcmp(getenv("TERM"), "dumb") == 0);
    CHECK(getenv("REMOVE_BEFORE_MAIN") == NULL);
    void* allocation = nrt_mem_alloc(19, 16, __FILE__, __LINE__);
    CHECK(nrt_mem_size(allocation) == 19);
    nrt_mem_free(allocation);
    constructor_ran = true;
}

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

int main(int argc, char** argv)
{
    CHECK(argc == 3 && constructor_ran);
    CHECK(metadata_provider() == 42);
    CHECK(g_nrt_process_metadata.no_colour == (argv[1][0] == '1'));
    CHECK(g_nrt_process_metadata.dumb_terminal == (argv[2][0] == '1'));
    check_parser();
    nrt_core_done();
    nrt_prn("runtime metadata passed", 23);
    return 0;
}
