#ifndef FORMAT_H
#define FORMAT_H

#include "list.h"
#include "options.h"

#include <stdbool.h>
#include <stddef.h>
#include <sys/stat.h>

void format_mode(mode_t mode, char *str);
void format_owner(uid_t uid, bool numeric, char *buf, size_t n);
void format_group(gid_t gid, bool numeric, char *buf, size_t n);
void format_size(const FileInfo *fi, const Options *opts,
                 char *buf, size_t n);
void format_blocks(const FileInfo *fi, const Options *opts,
                   char *buf, size_t n);
void format_time(const FileInfo *fi, const Options *opts,
                 char *buf, size_t n);

void print_total(long total_blocks, const Options *opts);
void print_name(const char *name, const Options *opts);
void print_indicator(mode_t mode);

#endif
