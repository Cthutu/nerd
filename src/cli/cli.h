//------------------------------------------------------------------------------
// Command-line parsing module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: core table object

#pragma once

#include <object/object.h>

//------------------------------------------------------------------------------
// LAYERING
//
// The CLI layer is generic and data-driven:
// - Input: a JSON schema that describes commands, flags, and parameters.
// - Input: an argc/argv pair.
// - Output: a JSON parse result describing the selected command and values.
//
// Validation is split intentionally:
// - CLI validates argv syntax against the schema. This includes unknown
//   commands/options, duplicate commands, missing option values, and positional
//   argument count mismatches.
// - Command-specific code validates semantics after parsing. This includes
//   domain rules such as required source files, incompatible options, or
//   command-specific defaults.
//
// In other words, CLI answers "what did the user type, according to the
// schema?" and command code answers "is that meaningful for this command?".
//
// SCHEMA FORMAT
//
// CLI schemas are JSON objects with this top-level shape:
// - "program": string
// - "summary": string
// - "flags": optional array of global flag objects
// - "params": optional array of global parameter objects
// - "commands": array of command objects
//
// Each command object may define:
// - "name": string
// - "summary": string
// - "flags": optional array of flag objects
// - "params": optional array of parameter objects
//
// Flag objects use:
// - "long": required long option name without leading "--"
// - "short": optional single-character short option without leading "-"
// - "description": optional help text
//
// Parameter objects use:
// - "name": logical parameter name used in the parse-result JSON
// - "kind": "named" or "positional"
// - "long": required for named params, omitted for positional params
// - "short": optional for named params
// - "description": optional help text
// - "required": whether the parameter must be present

typedef enum : u8 {
    CLI_PARAM_NAMED = 0,
    CLI_PARAM_POSITIONAL,
} CliParamKind;

typedef struct {
    char   short_name;
    string long_name;
    string description;
} CliFlag;

typedef struct {
    string       name;
    char         short_name;
    string       long_name;
    string       description;
    bool         required;
    CliParamKind kind;
} CliParam;

typedef struct {
    string name;
    string summary;
    Array(CliFlag) flags;
    Array(CliParam) params;
} CliCommand;

typedef struct {
    string program_name;
    string summary;
    Array(CliFlag) root_flags;
    Array(CliParam) root_params;
    Array(CliCommand) commands;
} CliParser;

void cli_init(CliParser* parser, const JsonValue* schema);
void cli_done(CliParser* parser);

JsonValue*
cli_parse(const CliParser* parser, Arena* arena, int argc, char** argv);
void cli_print_help(const CliParser* parser);
void cli_print_command_help(const CliParser* parser, usize command_index);

//------------------------------------------------------------------------------
