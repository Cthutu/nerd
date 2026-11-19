// Mutate the environment in a DSO constructor, before executable constructors.
#include <stdlib.h>

__attribute__((constructor)) static void mutate_environment(void)
{
    if (setenv("NO_COLOR", "1", 1) != 0 || setenv("TERM", "dumb", 1) != 0 ||
        unsetenv("REMOVE_BEFORE_MAIN") != 0) {
        _Exit(90);
    }
}

int metadata_provider(void) { return 42; }
