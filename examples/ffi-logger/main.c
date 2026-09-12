extern void myapp_log(const char* channel, const char* format, ...);

int main(void)
{
    myapp_log("render", "frame=%d time=%.2f name=%s", 42, 1.25, "main");
    return 0;
}
