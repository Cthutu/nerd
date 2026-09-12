# C calls a variadic Nerd DLL function

On Linux, from the repository root:

```sh
mkdir -p _bin/ffi-logger
nerd build --dll examples/ffi-logger/logger.n --output _bin/ffi-logger/logger.so
clang examples/ffi-logger/main.c _bin/ffi-logger/logger.so -Wl,-rpath,"$PWD/_bin/ffi-logger" -o _bin/ffi-logger/caller
_bin/ffi-logger/caller
```

Output: `frame=42 time=1.25 name=main`.

`pub myapp_log :: log_impl` selects the exact C export name. There is no `$`
prefix in the public function name. `args: ...` receives the C variadic tail;
`format_c` formats it into Nerd's temporary arena without advancing the cursor.
The logger restores its temporary arena mark after printing.
The compiler releases the cursor and its copies when the receiving function
returns. This example deliberately uses an application prefix to avoid C
library names such as `log`.

C++ callers should declare the function with `extern "C"`.
