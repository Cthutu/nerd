//------------------------------------------------------------------------------
// Directory helpers
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <core/core.h>

//------------------------------------------------------------------------------

#if OS_WINDOWS

bool dir_iter_init(DirIter* iter, cstr path)
{
    *iter       = (DirIter){0};
    iter->path  = path;

    Arena arena = {0};
    arena_init(&arena);
    cstr pattern  = path_join(&arena, path, "*");
    iter->pattern = (char*)ALLOC(strlen(pattern) + 1);
    strcpy(iter->pattern, pattern);
    iter->handle = FindFirstFileA(iter->pattern, &iter->find_data);
    if (iter->handle == INVALID_HANDLE_VALUE) {
        FREE(iter->pattern);
        arena_done(&arena);
        return false;
    }

    iter->first_ready = true;
    arena_done(&arena);
    return true;
}

bool dir_iter_next(DirIter* iter,
                   Arena*   arena,
                   cstr*    out_path,
                   bool*    out_is_directory)
{
    WIN32_FIND_DATAA* data = &iter->find_data;

    for (;;) {
        bool available = false;
        if (iter->first_ready) {
            iter->first_ready = false;
            available         = true;
        } else {
            available = FindNextFileA(iter->handle, data);
        }

        if (!available) {
            return false;
        }

        if (strcmp(data->cFileName, ".") == 0 ||
            strcmp(data->cFileName, "..") == 0) {
            continue;
        }

        *out_path = path_join(arena, iter->path, data->cFileName);
        if (out_is_directory) {
            *out_is_directory =
                (data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        }
        return true;
    }
}

void dir_iter_done(DirIter* iter)
{
    if (iter->handle && iter->handle != INVALID_HANDLE_VALUE) {
        FindClose(iter->handle);
    }
    if (iter->pattern) {
        FREE(iter->pattern);
    }
    *iter = (DirIter){0};
}

#elif OS_POSIX

bool dir_iter_init(DirIter* iter, cstr path)
{
    *iter     = (DirIter){0};
    iter->dir = opendir(path);
    if (!iter->dir) {
        return false;
    }

    iter->path = path;
    return true;
}

bool dir_iter_next(DirIter* iter,
                   Arena*   arena,
                   cstr*    out_path,
                   bool*    out_is_directory)
{
    for (;;) {
        struct dirent* entry = readdir(iter->dir);
        if (!entry) {
            return false;
        }

        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        *out_path = path_join(arena, iter->path, entry->d_name);
        if (out_is_directory) {
            *out_is_directory = path_is_directory(*out_path);
        }
        return true;
    }
}

void dir_iter_done(DirIter* iter)
{
    if (iter->dir) {
        closedir(iter->dir);
    }
    *iter = (DirIter){0};
}

#else
#    error "Directory iteration not implemented for this OS."
#endif

//------------------------------------------------------------------------------
