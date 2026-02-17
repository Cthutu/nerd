//------------------------------------------------------------------------------
// Elf tool
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: core elf

#include <elf/elf.h>

int run(int argc, char** argv)
{
    UNUSED(argc);

    cstr    program_name = argv[0];
    FileMap file;
    filemap_load(program_name, &file);

    prn("Loaded ELF file: %s (size: %zu bytes)\n", program_name, file.size);

    string elf_data = string_from(file.data, file.size);

    ElfHeader elf_header;
    elf_load(elf_data, &elf_header);
    elf_dump_header(&elf_header);
    elf_done(&elf_header);

    filemap_unload(&file);
    return 0;
}

//------------------------------------------------------------------------------
