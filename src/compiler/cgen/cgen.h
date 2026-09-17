#pragma once
#include <compiler/build/build.h>

// Render one self-contained translation unit from the checked program HIR.
bool cgen_save_program(const ProgramInfo*        program,
                       const NerdArtifactConfig* artifacts);
