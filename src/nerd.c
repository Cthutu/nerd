//
// Hello test
//
//> use: core intern compiler

#include <compiler/lexer/lexer.h>

DEF_SLICE(cstr) strings;

//------------------------------------------------------------------------------
// Front End

typedef struct {
    Lexer lexer;

    TimeDuration lex_time;
} FrontEndResults;

FrontEndResults front_end(string source_code)
{
    FrontEndResults results = {0};

    prn("Source code: " STRINGP, STRINGV(source_code));

    //
    // Lexing
    //

    prn("\n-- Lexing --");
    TimePoint lex_start = time_now();
    results.lexer       = lex(source_code);
    results.lex_time    = time_elapsed(lex_start, time_now());
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
    cstr output_base = "test";

    FrontEndResults front_end_results =
        front_end(string_from_cstr(source_code));

    UNUSED(libs);
    UNUSED(output_base);
    UNUSED(front_end_results);

    return 0;
}
