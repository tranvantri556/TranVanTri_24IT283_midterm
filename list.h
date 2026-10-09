#ifndef LIST_H
#define LIST_H

#include "options.h"

#include <stdbool.h>
#include <stddef.h>
#include <sys/stat.h>

typedef struct {
    char *name;
    char *path;
    struct stat st;
    bool stat_ok;
} FileInfo;

typedef struct {
    FileInfo *items;
    size_t len;
    size_t cap;
} FileList;

void list_main(Options *opts, int num_operands, char **operands);

void fl_init(FileList *fl);
void fl_add(FileList *fl, const char *name, const char *path,
            struct stat *st, bool stat_ok);
void fl_free(FileList *fl);

#endif
