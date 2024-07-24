#ifndef FIR_OS_H
#define FIR_OS_H

#include <fir/fir.h>
#include <stdbool.h>
#include <dirent.h>
#include <stdint.h>


#define ANSI_RESET "\033[0m"
#define ANSI_RED   "\033[1;31m"
#define ANSI_BLUE  "\033[34m"
#define ANSI_GREY  "\033[90m"


// @todo unify with arena
#define os_alloc_T(T) (T*)os_priv_alloc(sizeof(T))
#define os_alloc(T, size) (T*)os_priv_alloc(sizeof(T) * size)
void *os_priv_alloc(size_t size);


// == files ==
void filepath_extend(String *a, String b);
String filepath_join(String a, String b);

typedef struct OsReadFileResult {
    bool   ok;
    String src;
} OsReadFileResult;
OsReadFileResult os_read_file(String path);

bool os_is_dir(String path);

typedef DIR* Directory;
Directory os_open_dir(String path);
char *os_next_file_in_dir(Directory dir);
void os_close_dir(Directory dir);


#endif
