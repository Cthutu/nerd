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
cstr compiler_cmd_output_root(Arena* arena, string output_path, NerdSource source);
cstr compiler_cmd_sidecar_path(Arena* arena,
                               cstr   output_root,
                               cstr   extension);
cstr compiler_cmd_temp_binary_path(Arena* arena, cstr output_root);
bool               compile(NerdSource                source,
                           const NerdArtifactConfig* artifacts,
                           bool                      dump_compiler_state,
                           Timing*                   timing);

//------------------------------------------------------------------------------
