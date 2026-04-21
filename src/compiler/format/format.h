//------------------------------------------------------------------------------
// Source formatting module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/compiler.h>

//------------------------------------------------------------------------------

bool format_source(NerdSource source, Arena* arena, string* out_text);
bool format_file(cstr input_path, cstr output_path);

//------------------------------------------------------------------------------
