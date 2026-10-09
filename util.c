#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *xmalloc(size_t size) {
    void *p = malloc(size);
    if (!p) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    return p;
}

char *xstrdup(const char *s) {
    char *p = strdup(s);
    if (!p) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }
    return p;
}

char *join_path(const char *dir, const char *name) {
    size_t dlen = strlen(dir);
    size_t nlen = strlen(name);
    bool need_slash = (dlen > 0 && dir[dlen - 1] != '/');

    char *path = xmalloc(dlen + need_slash + nlen + 1);
    strcpy(path, dir);
    if (need_slash) {
        strcat(path, "/");
    }
    strcat(path, name);
    return path;
}

void human_readable(long long bytes, char *buf, size_t bufsz) {
    const char units[] = "BKMGTP";
    double size = (double)bytes;
    int i = 0;

    while (size >= 1024.0 && i < 5) {
        size /= 1024.0;
        i++;
    }

    if (i == 0) {
        snprintf(buf, bufsz, "%lldB", bytes);
    } else {
        snprintf(buf, bufsz, "%.1f%c", size, units[i]);
    }
}
