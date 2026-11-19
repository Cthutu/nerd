#pragma once
#include <compiler/build/build.h>

// Conservative source-level dependency query for standalone C link options.
bool cgen_requires_hosted_libraries(const ProgramInfo* program);

// Render one self-contained translation unit from the checked program HIR.
bool cgen_save_program(const ProgramInfo*        program,
                       const NerdArtifactConfig* artifacts);
