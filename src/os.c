#include "fir/os.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <sys/stat.h>

static char *path_to_cstr(String path, String ext) {
    // convert to null terminating
    char *cstr = malloc(path.len + ext.len + 1);
    memcpy(cstr, path.ptr, path.len);
    memcpy(cstr + path.len, ext.ptr, ext.len);
    cstr[path.len + ext.len] = '\0';

    return cstr;
}

void *os_priv_alloc(size_t size) {
    return malloc(size);
}

void filepath_extend(String *a, String b) {
    size_t len = a->len + b.len + 1;
    char *ptr = realloc(a->ptr, len+1);

    ptr[a->len] = '/';
    strncpy(ptr + a->len + 1, b.ptr, b.len);
    ptr[len] = '\0';

    a->ptr = ptr;
    a->len = len;
}

String filepath_join(String a, String b) {
    size_t len = a.len + b.len + 1;
    char *ptr = malloc(len+1);

    strncpy(ptr, a.ptr, a.len);
    ptr[a.len]= '/';
    strncpy(ptr + a.len + 1, b.ptr, b.len);
    ptr[len] = '\0';

    return (String){ .ptr = ptr, .len = len, .unique_id = 0 };
}

OsReadFileResult os_read_file(String path) {
    OsReadFileResult result;
    result.ok = false;

    char buffer[256];
    strncpy(buffer, path.ptr, path.len);
    buffer[path.len] = '\0';

    FILE *file = fopen(buffer, "rb");

    if (file == NULL) {
        return result;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        return result;
    }

    long file_len = ftell(file);
    if (file_len == -1) {
        return result;
    }

    char *ptr = malloc(sizeof(char)  *file_len + 1);
    if (ptr == NULL) {
        return result;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        free(ptr);
        return result;
    }

    long file_read = fread(ptr, sizeof(char), file_len, file);
    if (file_read != file_len) {
        free(ptr);
        return result;
    }

    fclose(file);

    result.ok = true;
    result.src = string_slc(ptr, file_len);

    return result;
}

bool os_is_dir(String path) {
    struct stat path_stat;
    char *path_cstr = path_to_cstr(path, string_lit(""));
    stat(path_cstr, &path_stat);
    free(path_cstr);

    return S_ISDIR(path_stat.st_mode);
}

Directory os_open_dir(String path) {
    char *path_cstr = path_to_cstr(path, string_lit(""));

    return opendir(path_cstr);
}

char *os_next_file_in_dir(Directory dir) {
    struct dirent *next;

    while ((next = readdir(dir)) != NULL) {
        if (next->d_type == DT_REG) {
            return next->d_name;
        }
    }

    return NULL;
}

void os_close_dir(Directory dir) {
    closedir(dir);
}
