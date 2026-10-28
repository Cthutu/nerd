/* A deterministic Xlib peer for the public frame lifecycle regression. */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <assert.h>

static unsigned   width, height;
static XSizeHints hints;
long              mock_hints_flags(void) { return hints.flags; }
Display*          XOpenDisplay(const char* name) { return (Display*)&width; }
int               XCloseDisplay(Display* d) { return 0; }
int               XDefaultScreen(Display* d) { return 0; }
Window            XRootWindow(Display* d, int screen) { return 1; }
unsigned long     XBlackPixel(Display* d, int screen) { return 0; }
Window            XCreateSimpleWindow(Display*      d,
                                      Window        parent,
                                      int           x,
                                      int           y,
                                      unsigned      w,
                                      unsigned      h,
                                      unsigned      border,
                                      unsigned long colour,
                                      unsigned long background)
{
    width  = w;
    height = h;
    return 2;
}
int    XStoreName(Display* d, Window w, const char* name) { return 0; }
int    XSelectInput(Display* d, Window w, long mask) { return 0; }
Atom   XInternAtom(Display* d, const char* name, Bool only) { return 1; }
Status XSetWMProtocols(Display* d, Window w, Atom* atoms, int count)
{
    return 1;
}
int  XMapWindow(Display* d, Window w) { return 0; }
void XSetWMNormalHints(Display* d, Window w, XSizeHints* value)
{
    hints = *value;
}
int XResizeWindow(Display* d, Window window, unsigned w, unsigned h)
{
    if (hints.flags) {
        assert(w == hints.min_width && w == hints.max_width);
        assert(h == hints.min_height && h == hints.max_height);
    }
    width  = w;
    height = h;
    return 0;
}
int XMoveResizeWindow(
    Display* d, Window window, int x, int y, unsigned w, unsigned h)
{
    assert(w == hints.min_width && w == hints.max_width);
    assert(h == hints.min_height && h == hints.max_height);
    width  = w;
    height = h;
    return 0;
}
Status
XSendEvent(Display* d, Window w, Bool propagate, long mask, XEvent* event)
{
    if (event->xclient.data.l[0] == 1) {
        assert(hints.flags == 0);
        width  = 1920;
        height = 1080;
    }
    return 1;
}
Status XGetGeometry(Display*  d,
                    Drawable  w,
                    Window*   root,
                    int*      x,
                    int*      y,
                    unsigned* out_width,
                    unsigned* out_height,
                    unsigned* border,
                    unsigned* depth)
{
    *root       = 1;
    *x          = 0;
    *y          = 0;
    *border     = 0;
    *depth      = 24;
    *out_width  = width;
    *out_height = height;
    return 1;
}
int XGetWindowProperty(Display* d,
                       Window   w,
                       Atom     property,
                       long     offset,
                       long     length,
                       Bool delete,
                       Atom            type,
                       Atom*           actual,
                       int*            format,
                       unsigned long*  count,
                       unsigned long*  after,
                       unsigned char** data)
{
    *data = NULL;
    return 1;
}
Bool XTranslateCoordinates(Display* d,
                           Window   source,
                           Window   dest,
                           int      x,
                           int      y,
                           int*     out_x,
                           int*     out_y,
                           Window*  child)
{
    return False;
}
int XDestroyWindow(Display* d, Window w) { return 0; }
int XFlush(Display* d) { return 0; }
int XSync(Display* d, Bool discard) { return 0; }
