#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdbool.h>

void *xmalloc(size_t size);
char *xstrdup(const char *s);
char *join_path(const char *dir, const char *name);
void human_readable(long long bytes, char *buf, size_t bufsz);

#endif
