//------------------------------------------------------------------------------
// ELF module API
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: core

#pragma once

#include <core/core.h>

//------------------------------------------------------------------------------
// ELF header

typedef enum : u8 {
    ELF_CLASS_32 = 1,
    ELF_CLASS_64 = 2,
} ElfClass;

typedef enum : u8 {
    ELF_DATA_LITTLE_ENDIAN = 1,
    ELF_DATA_BIG_ENDIAN    = 2,
} ElfEndianess;

typedef enum : u16 {
    ELF_ARCH_UNSPECIFIED = 0,
    ELF_ARCH_SPARC       = 2,
    ELF_ARCH_X86         = 3,
    ELF_ARCH_MIPS        = 8,
    ELF_ARCH_POWERPC     = 0x14,
    ELF_ARCH_ARM         = 0x28,
    ELF_ARCH_SUPERH      = 0x2a,
    ELF_ARCH_IA_64       = 0x32,
    ELF_ARCH_X86_64      = 0x3e,
    ELF_ARCH_AARCH64     = 0xb7,
    ELF_ARCH_RISCV       = 0xf3,
} ElfArchitecture;

typedef enum : u32 {
    ELF_PT_NULL    = 0,          // Unused segment
    ELF_PT_LOAD    = 1,          // Loadable segment
    ELF_PT_DYNAMIC = 2,          // Dynamic linking information
    ELF_PT_INTERP  = 3,          // Interpreter information
    ELF_PT_NOTE    = 4,          // Auxiliary information
    ELF_PT_SHLIB   = 5,          // Reserved
    ELF_PT_PHDR    = 6,          // Segment containing program header itself
    ELF_PT_TLS     = 7,          // Thread-local storage segment
    ELF_PT_LOOS    = 0x60000000, // Start of OS-specific
    ELF_PT_HIOS    = 0x6fffffff, // End of OS-specific
    ELF_PT_LOPROC  = 0x70000000, // Start of processor-specific
    ELF_PT_HIPROC  = 0x7fffffff, // End of processor-specific
} ElfProgramHeaderType;

typedef enum : u32 {
    ELF_FLAGS_EXECUTABLE = 1 << 0,
    ELF_FLAGS_WRITABLE   = 1 << 1,
    ELF_FLAGS_READABLE   = 1 << 2,
} ElfSegmentFlags;

typedef struct {
    ElfProgramHeaderType type;
    ElfSegmentFlags      flags;
    u64                  offset;
    u64                  virtual_addr;
    u64                  physical_addr;
    u64                  file_size;
    u64                  mem_size;
    u64                  align;
} ElfProgramHeader;

typedef enum : u32 {
    ELF_SHT_NULL     = 0,          // Inactive section
    ELF_SHT_PROGBITS = 1,          // Program data
    ELF_SHT_SYMTAB   = 2,          // Symbol table
    ELF_SHT_STRTAB   = 3,          // String table
    ELF_SHT_RELA     = 4,          // Relocation entries with addends
    ELF_SHT_HASH     = 5,          // Symbol hash table
    ELF_SHT_DYNAMIC  = 6,          // Dynamic linking information
    ELF_SHT_NOTE     = 7,          // Auxiliary information
    ELF_SHT_NOBITS   = 8,          // Program space with no data (bss)
    ELF_SHT_REL      = 9,          // Relocation entries without addends
    ELF_SHT_SHLIB    = 10,         // Reserved
    ELF_SHT_DYNSYM   = 11,         // Dynamic linker symbol table
    ELF_SHT_LOOS     = 0x60000000, // Start of OS-specific
    ELF_SHT_HIOS     = 0x6fffffff, // End of OS-specific
    ELF_SHT_LOPROC   = 0x70000000, // Start of processor-specific
    ELF_SHT_HIPROC   = 0x7fffffff, // End of processor-specific
} ElfSectionHeaderType;

typedef enum : u64 {
    ELF_SHF_WRITE            = 1 << 0,
    ELF_SHF_ALLOC            = 1 << 1,
    ELF_SHF_EXECINSTR        = 1 << 2,
    ELF_SHF_MERGE            = 1 << 4,
    ELF_SHF_STRINGS          = 1 << 5,
    ELF_SHF_INFO_LINK        = 1 << 6,
    ELF_SHF_LINK_ORDER       = 1 << 7,
    ELF_SHF_OS_NONCONFORMING = 1 << 8,
    ELF_SHF_GROUP            = 1 << 9,
    ELF_SHF_TLS              = 1 << 10,
    ELF_SHF_MASKOS           = 0x0ff00000, // OS-specific
    ELF_SHF_MASKPROC         = 0xf0000000, // Processor-specific
    ELF_ORDERED = 0x40000000, // Special ordering requirement (Solaris)
    ELF_EXCLUDE =
        0x80000000, // Section is excluded from process image (Solaris)
} ElfSectionHeaderFlags;

typedef struct {
    u32 name;         // Section name (string table index)
    u32 type;         // Section type
    u64 flags;        // Section flags
    u64 virtual_addr; // Section virtual address
    u64 file_offset;  // Section file offset
    u64 size;         // Section size in bytes
    u32 link;         // Link to another section
    u32 info;         // Additional section information
    u64 addralign;    // Section alignment
    u64 entsize;      // Entry size if section holds a table
} ElfSectionHeader;

typedef struct {
    ElfClass        bit_size;     // 32-bit or 64-bit
    ElfEndianess    endianess;    // Little-endian or big-endian
    u8              version;      // ELF version (should be always 1)
    u8              abi_version;  // ABI version
    ElfArchitecture architecture; // Target architecture (CPU)

    u64 entry_point;           // Entry point virtual address
    u64 program_header_offset; // Program header table file offset
    u64 section_header_offset; // Section header table file offset
    u32 flags;                 // Processor-specific flags
    u16 elf_header_size;       // ELF header size in bytes

    Array(ElfProgramHeader)
        program_headers; // Pointer to Program Header table in memory
    Array(ElfSectionHeader)
        section_headers; // Pointer to Section Header table in memory
} ElfHeader;

//------------------------------------------------------------------------------
// ELF interface

bool elf_load(string elf_data, ElfHeader* out_header);
void elf_done(ElfHeader* header);
void elf_dump_header(const ElfHeader* header);

//------------------------------------------------------------------------------
