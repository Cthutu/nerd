//------------------------------------------------------------------------------
// Compiler back-end module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/build/build.h>

//------------------------------------------------------------------------------

bool back_end_program(const ProgramInfo*        program,
                      const NerdArtifactConfig* artifacts,
                      bool                      verbose,
                      Timing*                   timing);

bool back_end_llvm_tool_output_self_test(void);
