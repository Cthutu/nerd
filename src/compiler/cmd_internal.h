//------------------------------------------------------------------------------
// Compiler command internals
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/internal.h>

//------------------------------------------------------------------------------

NerdArtifactConfig compiler_cmd_default_artifacts(void);
cstr               compiler_cmd_copy_path(Arena* arena, string path);
bool               compile(NerdSource                source,
                           const NerdArtifactConfig* artifacts,
                           bool                      dump_compiler_state,
                           Timing*                   timing);

//------------------------------------------------------------------------------
