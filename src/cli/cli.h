//------------------------------------------------------------------------------
// Command-line parsing module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: core table

#pragma once

#include <core/core.h>

//------------------------------------------------------------------------------

typedef struct {
    char short_name;
    cstr long_name;
    cstr description;
    bool* out_value;
} CliFlag;

typedef struct {
    cstr           program_name;
    cstr           summary;
    Array(CliFlag) flags;
} CliParser;

typedef struct {
    bool help_requested;
} CliParseResult;

void cli_init(CliParser* parser, cstr program_name, cstr summary);
void cli_done(CliParser* parser);

void cli_add_flag(CliParser* parser,
                  char       short_name,
                  cstr       long_name,
                  cstr       description,
                  bool*      out_value);

CliParseResult cli_parse(CliParser* parser, int argc, char** argv);
void           cli_print_help(const CliParser* parser);

//------------------------------------------------------------------------------
