//
// Hello test
//
//> use: core intern compiler timing

#include <compiler/lexer/lexer.h>
#include <timing/timing.h>

DEF_SLICE(cstr) strings;

//------------------------------------------------------------------------------
// Front End

typedef struct {
    Lexer lexer;
} FrontEndResults;

typedef struct {
} BackEndResults;

FrontEndResults front_end(string source_code, Timing* timing)
{
    FrontEndResults results = {0};

    prn("Source code: " STRINGP, STRINGV(source_code));

    //
    // Lexing
    //

    prn("\n-- Lexing --");
    TimePoint lex_start = time_now();
    results.lexer       = lex(source_code);
    TimePoint lex_end   = time_now();
    timing_add(timing,
               "front-end",
               "tokenise source text",
               time_elapsed(lex_start, lex_end));
    lex_dump(&results.lexer);

    //
    // Done
    //

    lex_done(&results.lexer);
    return results;
}

//------------------------------------------------------------------------------
// Back End

//------------------------------------------------------------------------------
// Entry point for the test.

int run(int argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);

    cstr source_code = "42";
    SLICE_SET(libs, cstr, "kernel32.lib");
    cstr   output_base = "test";
    Timing timing      = {0};
    timing_init(&timing);

    FrontEndResults front_end_results =
        front_end(string_from_cstr(source_code), &timing);

    timing_dump(&timing);

    UNUSED(libs);
    UNUSED(output_base);
    UNUSED(front_end_results);

    timing_done(&timing);

    return 0;
}
