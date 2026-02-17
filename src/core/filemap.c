//------------------------------------------------------------------------------
// File mapping API implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <core/core.h>

#if OS_POSIX
#    include <fcntl.h>
#    include <sys/mman.h>
#    include <sys/stat.h>
#endif

//------------------------------------------------------------------------------

u8* g_empty_file = (u8*)"";

#if OS_POSIX

string filemap_load(cstr path, FileMap* map)
{
    *map = (FileMap){0};

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        eprn("Failed to open file: %s\n", path);
        return (string){0};
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        eprn("Failed to get file size: %s\n", path);
        close(fd);
        return (string){0};
    }

    if (st.st_size < 0 || (u64)st.st_size > (u64)SIZE_MAX) {
        eprn("File size is invalid or too large: %s\n", path);
        close(fd);
        return (string){0};
    }

    map->size = (usize)st.st_size;
    if (map->size == 0) {
        map->data = g_empty_file;
        close(fd);
        return (string){
            .data  = map->data,
            .count = map->size,
        };
    }

    void* view = mmap(NULL, map->size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (view == MAP_FAILED) {
        eprn("Failed to map file to memory: %s\n", path);
        map->size = 0;
        return (string){0};
    }

    map->data = (u8*)view;
    return (string){
        .data  = map->data,
        .count = map->size,
    };
}

//------------------------------------------------------------------------------

void filemap_unload(FileMap* map)
{
    if (map && map->data) {
        if (map->data != g_empty_file && map->size != 0) {
            munmap(map->data, map->size);
        }
        map->data = NULL;
        map->size = 0;
    }
}

//------------------------------------------------------------------------------

#elif OS_WINDOWS

string filemap_load(cstr path, FileMap* map)
{
    *map        = (FileMap){0};

    HANDLE file = CreateFileA(path,
                              GENERIC_READ,
                              FILE_SHARE_READ,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                              NULL);
    if (file == INVALID_HANDLE_VALUE) {
        eprn("Failed to open file: %s\n", path);
        return (string){0};
    }

    LARGE_INTEGER file_size;
    if (!GetFileSizeEx(file, &file_size)) {
        eprn("Failed to get file size: %s\n", path);
        CloseHandle(file);
        return (string){0};
    }

    if (file_size.QuadPart < 0 || (u64)file_size.QuadPart > (u64)SIZE_MAX) {
        eprn("File size is invalid or too large: %s\n", path);
        CloseHandle(file);
        return (string){0};
    }

    map->size = (usize)file_size.QuadPart;
    if (map->size == 0) {
        map->data = g_empty_file;
        CloseHandle(file);
        return (string){
            .data  = map->data,
            .count = map->size,
        };
    }

    HANDLE mapping = CreateFileMappingA(file, NULL, PAGE_READONLY, 0, 0, NULL);
    if (!mapping) {
        eprn("Failed to create file mapping: %s\n", path);
        CloseHandle(file);
        return (string){0};
    }

    void* view = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
    CloseHandle(mapping);
    CloseHandle(file);

    if (!view) {
        eprn("Failed to map file to memory: %s\n", path);
        return (string){0};
    }

    map->data = (u8*)view;
    return (string){
        .data  = map->data,
        .count = map->size,
    };
}

void filemap_unload(FileMap* map)
{
    if (map && map->data) {
        if (map->data != g_empty_file && map->size != 0) {
            UnmapViewOfFile(map->data);
        }
        map->data = NULL;
        map->size = 0;
    }
}

//------------------------------------------------------------------------------

#else
#    error "File mapping not implemented for this OS."
#endif // OS_POSIX

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
