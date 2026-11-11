#include <SDL3/SDL.h>
#include <stdio.h>
#include "file.h"

#define MAX_FILES 64
static FILE *files[MAX_FILES];

FILE *File_Open(const char *path, const char *mode) {
    char normalized[512];
    char fullPath[512];
    size_t i;

    /* Decompiled call sites still build some paths with Windows separators. */
    for (i = 0; path[i] != '\0' && i < sizeof(normalized) - 1; i++) {
        normalized[i] = (path[i] == '\\') ? '/' : path[i];
    }
    normalized[i] = '\0';

    snprintf(fullPath, sizeof(fullPath), "assets/%s", normalized);
    return fopen(fullPath, mode);
}

Sint32 sOpenFile(char *path) {
    FILE *file = File_Open(path, "rb");
    if (!file) {
        SDL_Log("file not found: %s", path);
        return -1;
    }
    for (int i = 1; i < MAX_FILES; i++) {
        if (!files[i]) {
            files[i] = file;
            return i;
        }
    }
    fclose(file);
    return -1;
}

Sint32 sReadFile(Sint32 file, void *dst, Sint32 size) {
    if (file <= 0 || file >= MAX_FILES || !files[file]) {
        return -1;
    }
    return (Sint32)fread(dst, 1, (size_t)size, files[file]);
}

void sCloseFile(Sint32 file) {
    if (file > 0 && file < MAX_FILES && files[file]) {
        fclose(files[file]);
        files[file] = 0;
    }
}

Sint32 sGetFileSize(Sint32 file) {
    long current;
    long size;
    if (file <= 0 || file >= MAX_FILES || !files[file]) {
        return -1;
    }
    current = ftell(files[file]);
    fseek(files[file], 0, SEEK_END);
    size = ftell(files[file]);
    fseek(files[file], current, SEEK_SET);
    return (Sint32)size;
}
