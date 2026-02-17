//------------------------------------------------------------------------------
// Unicode helpers
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: core

#pragma once

#include <core/core.h>

//------------------------------------------------------------------------------

usize unicode_utf8_char_cell_width(string text, usize* bytes_consumed);
usize unicode_utf8_string_cell_width(string text);

//------------------------------------------------------------------------------
