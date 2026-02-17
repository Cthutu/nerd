//------------------------------------------------------------------------------
// ELF loading module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <elf/elf.h>

//------------------------------------------------------------------------------
// ELF header structures

// Contains header information up until it differs for 32-bit and 64-bit
START_PACKED_STRUCT(AgnosticElfHeader)
u32 e_magic;      // Magic number
u8  e_class;      // 1 = 32-bit, 2 = 64-bit
u8  e_data;       // 1 = little-endian, 2 = big-endian
u8  e_version;    // ELF header version
u8  e_osabi;      // OS ABI
u8  e_padding[8]; // Padding bytes
u16 e_type;       // Object file type
u16 e_machine;    // Architecture
u32 e_version2;   // ELF version
END_PACKED_STRUCT(AgnosticElfHeader)

// 32-bit version of second part of header
START_PACKED_STRUCT(Elf32Header)
u32 e_entry;     // Entry point virtual address
u32 e_phoff;     // Program header table file offset
u32 e_shoff;     // Section header table file offset
u32 e_flags;     // Processor-specific flags
u16 e_ehsize;    // ELF header size in bytes
u16 e_phentsize; // Program header table entry size
u16 e_phnum;     // Program header table entry count
u16 e_shentsize; // Section header table entry size
u16 e_shnum;     // Section header table entry count
u16 e_shstrndx;  // Section header string table index
END_PACKED_STRUCT(Elf32Header)

// 64-bit version of second part of header
START_PACKED_STRUCT(Elf64Header)
u64 e_entry;     // Entry point virtual address
u64 e_phoff;     // Program header table file offset
u64 e_shoff;     // Section header table file offset
u32 e_flags;     // Processor-specific flags
u16 e_ehsize;    // ELF header size in bytes
u16 e_phentsize; // Program header table entry size
u16 e_phnum;     // Program header table entry count
u16 e_shentsize; // Section header table entry size
u16 e_shnum;     // Section header table entry count
u16 e_shstrndx;  // Section header string table index
END_PACKED_STRUCT(Elf64Header)

// 32-bit version of program header
START_PACKED_STRUCT(Elf32ProgramHeader)
u32 p_type;   // Segment type
u32 p_offset; // Segment file offset
u32 p_vaddr;  // Segment virtual address
u32 p_paddr;  // Segment physical address
u32 p_filesz; // Segment size in file
u32 p_memsz;  // Segment size in memory
u32 p_flags;  // Segment flags
u32 p_align;  // Segment alignment
END_PACKED_STRUCT(Elf32ProgramHeader)

// 64-bit version of program header
START_PACKED_STRUCT(Elf64ProgramHeader)
u32 p_type;   // Segment type
u32 p_flags;  // Segment flags
u64 p_offset; // Segment file offset
u64 p_vaddr;  // Segment virtual address
u64 p_paddr;  // Segment physical address
u64 p_filesz; // Segment size in file
u64 p_memsz;  // Segment size in memory
u64 p_align;  // Segment alignment
END_PACKED_STRUCT(Elf64ProgramHeader)

// 32-bit version of section header
START_PACKED_STRUCT(Elf32SectionHeader)
u32 sh_name;      // Section name (string table index)
u32 sh_type;      // Section type
u32 sh_flags;     // Section flags
u32 sh_addr;      // Section virtual address
u32 sh_offset;    // Section file offset
u32 sh_size;      // Section size in bytes
u32 sh_link;      // Link to another section
u32 sh_info;      // Additional section information
u32 sh_addralign; // Section alignment
u32 sh_entsize;   // Entry size if section holds a table
END_PACKED_STRUCT(Elf32SectionHeader)

// 64-bit version of section header
START_PACKED_STRUCT(Elf64SectionHeader)
u32 sh_name;      // Section name (string table index)
u32 sh_type;      // Section type
u64 sh_flags;     // Section flags
u64 sh_addr;      // Section virtual address
u64 sh_offset;    // Section file offset
u64 sh_size;      // Section size in bytes
u32 sh_link;      // Link to another section
u32 sh_info;      // Additional section information
u64 sh_addralign; // Section alignment
u64 sh_entsize;   // Entry size if section holds a table
END_PACKED_STRUCT(Elf64SectionHeader)

//------------------------------------------------------------------------------
// ELF loading functions

bool elf_load(string elf_data, ElfHeader* out_header)
{
    //
    // Process the ELF file header
    //

    if (elf_data.count < sizeof(AgnosticElfHeader)) {
        eprn("ELF data too small to contain header\n");
        return false;
    }

    AgnosticElfHeader* agnostic_header = (AgnosticElfHeader*)elf_data.data;

    // Verify magic number
    if (agnostic_header->e_magic != 0x464C457F) {
        eprn("Invalid ELF magic number\n");
        return false;
    }

    // Check endianess (not supporting different endianess for now)
    if (agnostic_header->e_data != 1) {
        eprn("Unsupported ELF endianess: %u\n", agnostic_header->e_data);
        return false;
    }

    out_header->bit_size     = (ElfClass)agnostic_header->e_class;
    out_header->endianess    = (ElfEndianess)agnostic_header->e_data;
    out_header->version      = agnostic_header->e_version;
    out_header->abi_version  = agnostic_header->e_osabi;
    out_header->architecture = (ElfArchitecture)agnostic_header->e_machine;

    u16 num_program_headers  = 0;
    u16 num_section_headers  = 0;

    if (out_header->bit_size == ELF_CLASS_32) {
        if (elf_data.count < sizeof(AgnosticElfHeader) + sizeof(Elf32Header)) {
            eprn("ELF data too small to contain 32-bit header\n");
            return false;
        }

        Elf32Header* elf32_header =
            (Elf32Header*)(elf_data.data + sizeof(AgnosticElfHeader));

        out_header->entry_point           = elf32_header->e_entry;
        out_header->program_header_offset = elf32_header->e_phoff;
        out_header->section_header_offset = elf32_header->e_shoff;
        out_header->flags                 = elf32_header->e_flags;
        out_header->elf_header_size       = elf32_header->e_ehsize;
        num_program_headers               = elf32_header->e_phnum;
        num_section_headers               = elf32_header->e_shnum;

        if (elf32_header->e_phentsize != sizeof(Elf32ProgramHeader)) {
            eprn("Invalid Program Header entry size: %u\n, should be %zu\n",
                 elf32_header->e_phentsize,
                 sizeof(Elf32ProgramHeader));
            return false;
        }
        if (elf32_header->e_shentsize != sizeof(Elf32SectionHeader)) {
            eprn("Invalid Section Header entry size: %u\n, should be %zu\n",
                 elf32_header->e_shentsize,
                 sizeof(Elf32SectionHeader));
            return false;
        }

    } else if (out_header->bit_size == ELF_CLASS_64) {
        if (elf_data.count < sizeof(AgnosticElfHeader) + sizeof(Elf64Header)) {
            eprn("ELF data too small to contain 64-bit header\n");
            return false;
        }

        Elf64Header* elf64_header =
            (Elf64Header*)(elf_data.data + sizeof(AgnosticElfHeader));

        out_header->entry_point           = elf64_header->e_entry;
        out_header->program_header_offset = elf64_header->e_phoff;
        out_header->section_header_offset = elf64_header->e_shoff;
        out_header->flags                 = elf64_header->e_flags;
        out_header->elf_header_size       = elf64_header->e_ehsize;
        num_program_headers               = elf64_header->e_phnum;
        num_section_headers               = elf64_header->e_shnum;

        if (elf64_header->e_phentsize != sizeof(Elf64ProgramHeader)) {
            eprn("Invalid Program Header entry size: %u\n, should be %zu\n",
                 elf64_header->e_phentsize,
                 sizeof(Elf64ProgramHeader));
            return false;
        }
        if (elf64_header->e_shentsize != sizeof(Elf64SectionHeader)) {
            eprn("Invalid Section Header entry size: %u\n, should be %zu\n",
                 elf64_header->e_shentsize,
                 sizeof(Elf64SectionHeader));
            return false;
        }

    } else {
        eprn("Unknown ELF class: %u\n", out_header->bit_size);
        return false;
    }

    //
    // Process the program headers
    //

    array_requires_size(out_header->program_headers, num_program_headers);
    ASSERT(array_count(out_header->program_headers) == num_program_headers,
           "Program headers reservation failed");
    for (u16 i = 0; i < num_program_headers; i++) {
        if (out_header->bit_size == ELF_CLASS_32) {
            Elf32ProgramHeader* ph =
                (Elf32ProgramHeader*)(elf_data.data +
                                      out_header->program_header_offset +
                                      i * sizeof(Elf32ProgramHeader));

            out_header->program_headers[i].type =
                (ElfProgramHeaderType)ph->p_type;
            out_header->program_headers[i].flags = (ElfSegmentFlags)ph->p_flags;
            out_header->program_headers[i].offset        = ph->p_offset;
            out_header->program_headers[i].virtual_addr  = ph->p_vaddr;
            out_header->program_headers[i].physical_addr = ph->p_paddr;
            out_header->program_headers[i].file_size     = ph->p_filesz;
            out_header->program_headers[i].mem_size      = ph->p_memsz;
            out_header->program_headers[i].align         = ph->p_align;
        } else if (out_header->bit_size == ELF_CLASS_64) {
            Elf64ProgramHeader* ph =
                (Elf64ProgramHeader*)(elf_data.data +
                                      out_header->program_header_offset +
                                      i * sizeof(Elf64ProgramHeader));

            out_header->program_headers[i].type =
                (ElfProgramHeaderType)ph->p_type;
            out_header->program_headers[i].flags = (ElfSegmentFlags)ph->p_flags;
            out_header->program_headers[i].offset        = ph->p_offset;
            out_header->program_headers[i].virtual_addr  = ph->p_vaddr;
            out_header->program_headers[i].physical_addr = ph->p_paddr;
            out_header->program_headers[i].file_size     = ph->p_filesz;
            out_header->program_headers[i].mem_size      = ph->p_memsz;
            out_header->program_headers[i].align         = ph->p_align;
        }
    }

    //
    // Process the section headers
    //

    array_requires_size(out_header->section_headers, num_section_headers);
    for (u16 i = 0; i < num_section_headers; i++) {
        if (out_header->bit_size == ELF_CLASS_32) {
            Elf32SectionHeader* sh =
                (Elf32SectionHeader*)(elf_data.data +
                                      out_header->section_header_offset +
                                      i * sizeof(Elf32SectionHeader));

            out_header->section_headers[i].name         = sh->sh_name;
            out_header->section_headers[i].type         = sh->sh_type;
            out_header->section_headers[i].flags        = sh->sh_flags;
            out_header->section_headers[i].virtual_addr = sh->sh_addr;
            out_header->section_headers[i].file_offset  = sh->sh_offset;
            out_header->section_headers[i].size         = sh->sh_size;
            out_header->section_headers[i].link         = sh->sh_link;
            out_header->section_headers[i].info         = sh->sh_info;
            out_header->section_headers[i].addralign    = sh->sh_addralign;
            out_header->section_headers[i].entsize      = sh->sh_entsize;

        } else if (out_header->bit_size == ELF_CLASS_64) {
            Elf64SectionHeader* sh =
                (Elf64SectionHeader*)(elf_data.data +
                                      out_header->section_header_offset +
                                      i * sizeof(Elf64SectionHeader));

            out_header->section_headers[i].name         = sh->sh_name;
            out_header->section_headers[i].type         = sh->sh_type;
            out_header->section_headers[i].flags        = sh->sh_flags;
            out_header->section_headers[i].virtual_addr = sh->sh_addr;
            out_header->section_headers[i].file_offset  = sh->sh_offset;
            out_header->section_headers[i].size         = sh->sh_size;
            out_header->section_headers[i].link         = sh->sh_link;
            out_header->section_headers[i].info         = sh->sh_info;
            out_header->section_headers[i].addralign    = sh->sh_addralign;
            out_header->section_headers[i].entsize      = sh->sh_entsize;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// ELF cleanup

void elf_done(ElfHeader* header)
{
    array_free(header->program_headers);
    array_free(header->section_headers);
}

//------------------------------------------------------------------------------
// ELF Header dump

internal string elf_program_header_type_string(u32 type, Arena* str_arena)
{
    switch (type) {
    case ELF_PT_NULL:
        return string_from_cstr("NULL");
    case ELF_PT_LOAD:
        return string_from_cstr("LOAD");
    case ELF_PT_DYNAMIC:
        return string_from_cstr("DYNAMIC");
    case ELF_PT_INTERP:
        return string_from_cstr("INTERP");
    case ELF_PT_NOTE:
        return string_from_cstr("NOTE");
    case ELF_PT_SHLIB:
        return string_from_cstr("SHLIB");
    case ELF_PT_PHDR:
        return string_from_cstr("PHDR");
    case ELF_PT_TLS:
        return string_from_cstr("TLS");
    default:
        {
            StringBuilder sb;
            sb_init(&sb, str_arena);
            sb_format(&sb, "0x%x", type);
            return sb_to_string(&sb);
        }
    }
}

internal string elf_section_type_string(u32 type, Arena* str_arena)
{
    switch (type) {
    case ELF_SHT_NULL:
        return string_from_cstr("NULL");
    case ELF_SHT_PROGBITS:
        return string_from_cstr("PROGBITS");
    case ELF_SHT_SYMTAB:
        return string_from_cstr("SYMTAB");
    case ELF_SHT_STRTAB:
        return string_from_cstr("STRTAB");
    case ELF_SHT_RELA:
        return string_from_cstr("RELA");
    case ELF_SHT_HASH:
        return string_from_cstr("HASH");
    case ELF_SHT_DYNAMIC:
        return string_from_cstr("DYNAMIC");
    case ELF_SHT_NOTE:
        return string_from_cstr("NOTE");
    case ELF_SHT_NOBITS:
        return string_from_cstr("NOBITS");
    case ELF_SHT_REL:
        return string_from_cstr("REL");
    case ELF_SHT_SHLIB:
        return string_from_cstr("SHLIB");
    case ELF_SHT_DYNSYM:
        return string_from_cstr("DYNSYM");
    default:
        {
            StringBuilder sb;
            sb_init(&sb, str_arena);
            sb_format(&sb, "0x%x", type);
            return sb_to_string(&sb);
        }
    }
}

void elf_dump_header(const ElfHeader* header)
{
    cstr architecture_strings[] = {
        [ELF_ARCH_UNSPECIFIED] = "Unspecified",
        [ELF_ARCH_SPARC]       = "SPARC",
        [ELF_ARCH_X86]         = "x86",
        [ELF_ARCH_MIPS]        = "MIPS",
        [ELF_ARCH_POWERPC]     = "PowerPC",
        [ELF_ARCH_ARM]         = "ARM",
        [ELF_ARCH_SUPERH]      = "SuperH",
        [ELF_ARCH_IA_64]       = "IA-64",
        [ELF_ARCH_X86_64]      = "x86-64",
        [ELF_ARCH_AARCH64]     = "AArch64",
        [ELF_ARCH_RISCV]       = "RISC-V",
    };
    prn("ELF Header Dump:");
    prn("  Bit Size: %s",
        header->bit_size == ELF_CLASS_32 ? "32-bit" : "64-bit");
    prn("  Endianess: %s",
        header->endianess == ELF_DATA_LITTLE_ENDIAN ? "Little-endian"
                                                    : "Big-endian");
    prn("  Version: %u", header->version);
    prn("  ABI Version: %u", header->abi_version);
    prn("  Architecture: %s",
        architecture_strings[header->architecture]
            ? architecture_strings[header->architecture]
            : "Unknown");

    prn("  Entry Point: 0x%llx", header->entry_point);
    prn("  Program Header Offset: 0x%llx", header->program_header_offset);
    prn("  Section Header Offset: 0x%llx", header->section_header_offset);
    prn("  Flags: 0x%x", header->flags);
    prn("  ELF Header Size: %u bytes", header->elf_header_size);
    prn("  Number of Program Headers: %u",
        array_count(header->program_headers));
    prn("  Number of Section Headers: %u",
        array_count(header->section_headers));

    prn("");

    Arena text_arena;
    arena_init(&text_arena);

    //
    // Output the program headers
    //

    prn("Program Headers:");
    prn(ANSI_FAINT
        "  Flags legend: R=READABLE W=WRITABLE X=EXECUTABLE" ANSI_RESET);
    prn(ANSI_BOLD ANSI_GREEN
        "%-3s " ANSI_RESET "|" ANSI_BOLD ANSI_GREEN " %-12s " ANSI_RESET
        "|" ANSI_BOLD ANSI_GREEN " %-3s " ANSI_RESET "|" ANSI_BOLD ANSI_GREEN
        " %-18s " ANSI_RESET "|" ANSI_BOLD ANSI_GREEN " %-18s " ANSI_RESET
        "|" ANSI_BOLD ANSI_GREEN " %-18s " ANSI_RESET "|" ANSI_BOLD ANSI_GREEN
        " %-18s " ANSI_RESET "|" ANSI_BOLD ANSI_GREEN " %-18s " ANSI_RESET
        "|" ANSI_BOLD ANSI_GREEN " %-18s " ANSI_RESET,
        "Idx",
        "Type",
        "Flg",
        "Offset",
        "VAddr",
        "PAddr",
        "FileSz",
        "MemSz",
        "Align");
    prn("----|--------------|-----|--------------------|--------------------|"
        "--------------------|--------------------|--------------------|"
        "--------------------");

    for (usize i = 0; i < array_count(header->program_headers); i++) {
        const ElfProgramHeader* ph = &header->program_headers[i];
        string                  type_text =
            elf_program_header_type_string(ph->type, &text_arena);

        char flags[4] = {'-', '-', '-', '\0'};
        if (ph->flags & ELF_FLAGS_READABLE) {
            flags[0] = 'R';
        }
        if (ph->flags & ELF_FLAGS_WRITABLE) {
            flags[1] = 'W';
        }
        if (ph->flags & ELF_FLAGS_EXECUTABLE) {
            flags[2] = 'X';
        }

        prn(ANSI_CYAN
            "%-3zu " ANSI_RESET "|" ANSI_YELLOW " %-12.*s " ANSI_RESET
            "|" ANSI_MAGENTA " %-3s " ANSI_RESET "|" ANSI_BLUE
            " 0x%016llx " ANSI_RESET "|" ANSI_BLUE " 0x%016llx " ANSI_RESET
            "|" ANSI_BLUE " 0x%016llx " ANSI_RESET "|" ANSI_BLUE
            " 0x%016llx " ANSI_RESET "|" ANSI_BLUE " 0x%016llx " ANSI_RESET
            "|" ANSI_BLUE " 0x%016llx " ANSI_RESET,
            i,
            STRINGV(type_text),
            flags,
            ph->offset,
            ph->virtual_addr,
            ph->physical_addr,
            ph->file_size,
            ph->mem_size,
            ph->align);

        arena_reset(&text_arena);
    }

    prn("");

    //
    // Output the section headers
    //

    prn("Section Headers:");
    prn(ANSI_FAINT
        "  Flags legend: W=WRITE A=ALLOC X=EXECINSTR M=MERGE "
        "S=STRINGS I=INFO_LINK L=LINK_ORDER O=OS_NONCONFORMING G=GROUP "
        "T=TLS" ANSI_RESET);
    prn(ANSI_BOLD ANSI_GREEN
        "%-3s " ANSI_RESET "|" ANSI_BOLD ANSI_GREEN " %-5s " ANSI_RESET
        "|" ANSI_BOLD ANSI_GREEN " %-12s " ANSI_RESET "|" ANSI_BOLD ANSI_GREEN
        " %-10s " ANSI_RESET "|" ANSI_BOLD ANSI_GREEN " %-18s " ANSI_RESET
        "|" ANSI_BOLD ANSI_GREEN " %-18s " ANSI_RESET "|" ANSI_BOLD ANSI_GREEN
        " %-18s " ANSI_RESET "|" ANSI_BOLD ANSI_GREEN " %-5s " ANSI_RESET
        "|" ANSI_BOLD ANSI_GREEN " %-5s " ANSI_RESET "|" ANSI_BOLD ANSI_GREEN
        " %-18s " ANSI_RESET "|" ANSI_BOLD ANSI_GREEN " %-18s " ANSI_RESET,
        "Idx",
        "Name",
        "Type",
        "Flags",
        "Addr",
        "Off",
        "Size",
        "Link",
        "Info",
        "Align",
        "EntSz");
    prn("----|-------|--------------|------------|--------------------|"
        "--------------------|--------------------|-------|-------|"
        "--------------------|--------------------");

    for (usize i = 0; i < array_count(header->section_headers); i++) {
        const ElfSectionHeader* sh = &header->section_headers[i];
        string type_text = elf_section_type_string(sh->type, &text_arena);

        char flags[11]   = {
            '-',
            '-',
            '-',
            '-',
            '-',
            '-',
            '-',
            '-',
            '-',
            '-',
            '\0',
        };
        if (sh->flags & ELF_SHF_WRITE) {
            flags[0] = 'W';
        }
        if (sh->flags & ELF_SHF_ALLOC) {
            flags[1] = 'A';
        }
        if (sh->flags & ELF_SHF_EXECINSTR) {
            flags[2] = 'X';
        }
        if (sh->flags & ELF_SHF_MERGE) {
            flags[3] = 'M';
        }
        if (sh->flags & ELF_SHF_STRINGS) {
            flags[4] = 'S';
        }
        if (sh->flags & ELF_SHF_INFO_LINK) {
            flags[5] = 'I';
        }
        if (sh->flags & ELF_SHF_LINK_ORDER) {
            flags[6] = 'L';
        }
        if (sh->flags & ELF_SHF_OS_NONCONFORMING) {
            flags[7] = 'O';
        }
        if (sh->flags & ELF_SHF_GROUP) {
            flags[8] = 'G';
        }
        if (sh->flags & ELF_SHF_TLS) {
            flags[9] = 'T';
        }

        prn(ANSI_CYAN
            "%-3zu " ANSI_RESET "|" ANSI_YELLOW " %-5u " ANSI_RESET
            "|" ANSI_YELLOW " %-12.*s " ANSI_RESET "|" ANSI_MAGENTA
            " %-10s " ANSI_RESET "|" ANSI_BLUE " 0x%016llx " ANSI_RESET
            "|" ANSI_BLUE " 0x%016llx " ANSI_RESET "|" ANSI_BLUE
            " 0x%016llx " ANSI_RESET "|" ANSI_YELLOW " %-5u " ANSI_RESET
            "|" ANSI_YELLOW " %-5u " ANSI_RESET "|" ANSI_BLUE
            " 0x%016llx " ANSI_RESET "|" ANSI_BLUE " 0x%016llx " ANSI_RESET,
            i,
            sh->name,
            STRINGV(type_text),
            flags,
            sh->virtual_addr,
            sh->file_offset,
            sh->size,
            sh->link,
            sh->info,
            sh->addralign,
            sh->entsize);

        arena_reset(&text_arena);
    }
}

//------------------------------------------------------------------------------
