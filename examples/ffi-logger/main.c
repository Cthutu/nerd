/* C host for the Nerd logging example. Build the companion logger.n as an
 * object or library and link it into this executable. The exported myapp_log
 * symbol uses the C calling convention, so the host calls it like a C function.
 * The format string describes the types of the following variadic arguments. */

extern void myapp_log(const char* channel, const char* format, ...);

int main(void)
{
    /* Nerd receives the zero-terminated strings and formats these arguments
     * during the call; the host does not retain any Nerd-owned result. */
    myapp_log("render", "frame=%d time=%.2f name=%s", 42, 1.25, "main");
    return 0;
}
