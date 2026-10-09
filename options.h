#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>

typedef enum {
    SIZE_DEFAULT,
    SIZE_KB,
    SIZE_HUMAN
} SizeMode;

typedef enum {
    TIME_MTIME,
    TIME_ATIME,
    TIME_CTIME
} TimeMode;

typedef enum {
    NONPRINT_DEFAULT,
    NONPRINT_RAW,
    NONPRINT_QUESTION
} NonprintMode;

typedef struct {
    bool show_all;
    bool show_almost_all;
    bool directory_plain;
    bool indicator;
    bool no_sort;
    bool inode;
    bool long_format;
    bool numeric;
    bool recursive;
    bool reverse;
    bool sort_size;
    bool blocks;
    bool sort_time;

    SizeMode size_mode;
    TimeMode time_mode;
    NonprintMode nonprint_mode;
} Options;

void options_init(Options *opts);
int options_parse(Options *opts, int argc, char **argv);

#endif
