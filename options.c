#include "options.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void options_init(Options *opts) {
    opts->show_all = false;
    opts->show_almost_all = false;
    opts->directory_plain = false;
    opts->indicator = false;
    opts->no_sort = false;
    opts->inode = false;
    opts->long_format = false;
    opts->numeric = false;
    opts->recursive = false;
    opts->reverse = false;
    opts->sort_size = false;
    opts->blocks = false;
    opts->sort_time = false;

    opts->size_mode = SIZE_DEFAULT;
    opts->time_mode = TIME_MTIME;
    opts->nonprint_mode = NONPRINT_DEFAULT;
}

int options_parse(Options *opts, int argc, char **argv) {
    int ch;

    while ((ch = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1) {
        switch (ch) {
        case 'A':
            opts->show_almost_all = true;
            opts->show_all = false;
            break;
        case 'a':
            opts->show_all = true;
            opts->show_almost_all = false;
            break;
        case 'c':
            opts->time_mode = TIME_CTIME;
            break;
        case 'd':
            opts->directory_plain = true;
            opts->recursive = false;
            break;
        case 'F':
            opts->indicator = true;
            break;
        case 'f':
            opts->no_sort = true;
            break;
        case 'h':
            opts->size_mode = SIZE_HUMAN;
            break;
        case 'i':
            opts->inode = true;
            break;
        case 'k':
            opts->size_mode = SIZE_KB;
            break;
        case 'l':
            opts->long_format = true;
            opts->numeric = false;
            break;
        case 'n':
            opts->long_format = true;
            opts->numeric = true;
            break;
        case 'q':
            opts->nonprint_mode = NONPRINT_QUESTION;
            break;
        case 'R':
            opts->recursive = true;
            opts->directory_plain = false;
            break;
        case 'r':
            opts->reverse = true;
            break;
        case 'S':
            opts->sort_size = true;
            opts->sort_time = false;
            break;
        case 's':
            opts->blocks = true;
            break;
        case 't':
            opts->sort_time = true;
            opts->sort_size = false;
            break;
        case 'u':
            opts->time_mode = TIME_ATIME;
            break;
        case 'w':
            opts->nonprint_mode = NONPRINT_RAW;
            break;
        default:
            fprintf(stderr,
                    "Usage: %s [-AacdFfhiklnqRrSstuw] [file ...]\n",
                    argv[0]);
            return -1;
        }
    }

    if (opts->nonprint_mode == NONPRINT_DEFAULT) {
        opts->nonprint_mode = isatty(STDOUT_FILENO)
                                ? NONPRINT_QUESTION
                                : NONPRINT_RAW;
    }

    return optind;
}
