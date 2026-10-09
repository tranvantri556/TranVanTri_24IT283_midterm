#include "format.h"
#include "util.h"

#include <ctype.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void format_mode(mode_t mode, char *str) {
    if (S_ISREG(mode)) {
        str[0] = '-';
    } else if (S_ISDIR(mode)) {
        str[0] = 'd';
    } else if (S_ISLNK(mode)) {
        str[0] = 'l';
    } else if (S_ISBLK(mode)) {
        str[0] = 'b';
    } else if (S_ISCHR(mode)) {
        str[0] = 'c';
    } else if (S_ISFIFO(mode)) {
        str[0] = 'p';
    } else if (S_ISSOCK(mode)) {
        str[0] = 's';
#ifdef S_ISWHT
    } else if (S_ISWHT(mode)) {
        str[0] = 'w';
#endif
    } else {
        str[0] = '?';
    }

    str[1] = (mode & S_IRUSR) ? 'r' : '-';
    str[2] = (mode & S_IWUSR) ? 'w' : '-';
    if (mode & S_ISUID) {
        str[3] = (mode & S_IXUSR) ? 's' : 'S';
    } else {
        str[3] = (mode & S_IXUSR) ? 'x' : '-';
    }

    str[4] = (mode & S_IRGRP) ? 'r' : '-';
    str[5] = (mode & S_IWGRP) ? 'w' : '-';
    if (mode & S_ISGID) {
        str[6] = (mode & S_IXGRP) ? 's' : 'S';
    } else {
        str[6] = (mode & S_IXGRP) ? 'x' : '-';
    }

    str[7] = (mode & S_IROTH) ? 'r' : '-';
    str[8] = (mode & S_IWOTH) ? 'w' : '-';
    if (mode & S_ISVTX) {
        str[9] = (mode & S_IXOTH) ? 't' : 'T';
    } else {
        str[9] = (mode & S_IXOTH) ? 'x' : '-';
    }

    str[10] = '\0';
}

void format_owner(uid_t uid, bool numeric, char *buf, size_t n) {
    if (numeric) {
        snprintf(buf, n, "%u", (unsigned)uid);
        return;
    }

    struct passwd *pw = getpwuid(uid);
    if (pw) {
        snprintf(buf, n, "%s", pw->pw_name);
    } else {
        snprintf(buf, n, "%u", (unsigned)uid);
    }
}

void format_group(gid_t gid, bool numeric, char *buf, size_t n) {
    if (numeric) {
        snprintf(buf, n, "%u", (unsigned)gid);
        return;
    }

    struct group *gr = getgrgid(gid);
    if (gr) {
        snprintf(buf, n, "%s", gr->gr_name);
    } else {
        snprintf(buf, n, "%u", (unsigned)gid);
    }
}

void format_size(const FileInfo *fi, const Options *opts,
                 char *buf, size_t n) {
    if (S_ISCHR(fi->st.st_mode) || S_ISBLK(fi->st.st_mode)) {
        snprintf(buf, n, "%u, %u",
                 major(fi->st.st_rdev),
                 minor(fi->st.st_rdev));
    } else if (opts->size_mode == SIZE_HUMAN) {
        human_readable(fi->st.st_size, buf, n);
    } else {
        snprintf(buf, n, "%lld", (long long)fi->st.st_size);
    }
}

void format_blocks(const FileInfo *fi, const Options *opts,
                   char *buf, size_t n) {
    long blocks = fi->st.st_blocks;

    if (opts->size_mode == SIZE_HUMAN) {
        human_readable((long long)blocks * 512, buf, n);
    } else if (opts->size_mode == SIZE_KB) {
        snprintf(buf, n, "%ld", (blocks + 1) / 2);
    } else {
        long bs = 512;
        const char *env = getenv("BLOCKSIZE");
        if (env) {
            long v = atol(env);
            if (v > 0) {
                bs = v;
            }
        }
        snprintf(buf, n, "%ld", (blocks * 512 + bs - 1) / bs);
    }
}

void format_time(const FileInfo *fi, const Options *opts,
                 char *buf, size_t n) {
    time_t t;

    switch (opts->time_mode) {
    case TIME_ATIME:
        t = fi->st.st_atime;
        break;
    case TIME_CTIME:
        t = fi->st.st_ctime;
        break;
    default:
        t = fi->st.st_mtime;
        break;
    }

    struct tm *tm = localtime(&t);
    if (tm) {
        strftime(buf, n, "%b %e %H:%M", tm);
    } else {
        snprintf(buf, n, "??? ?? ??:??");
    }
}

void print_total(long total_blocks, const Options *opts) {
    char buf[64];

    if (opts->size_mode == SIZE_HUMAN) {
        human_readable((long long)total_blocks * 512, buf, sizeof(buf));
        printf("total %s\n", buf);
    } else if (opts->size_mode == SIZE_KB) {
        printf("total %ld\n", (total_blocks + 1) / 2);
    } else {
        long bs = 512;
        const char *env = getenv("BLOCKSIZE");
        if (env) {
            long v = atol(env);
            if (v > 0) {
                bs = v;
            }
        }
        long total = (total_blocks * 512 + bs - 1) / bs;
        printf("total %ld\n", total);
    }
}

void print_name(const char *name, const Options *opts) {
    if (opts->nonprint_mode == NONPRINT_RAW) {
        fputs(name, stdout);
    } else {
        for (const unsigned char *p = (const unsigned char *)name;
             *p; p++) {
            if (isprint(*p)) {
                putchar(*p);
            } else {
                putchar('?');
            }
        }
    }
}

void print_indicator(mode_t mode) {
    if (S_ISDIR(mode)) {
        putchar('/');
    } else if (S_ISLNK(mode)) {
        putchar('@');
    } else if (S_ISSOCK(mode)) {
        putchar('=');
    } else if (S_ISFIFO(mode)) {
        putchar('|');
#ifdef S_ISWHT
    } else if (S_ISWHT(mode)) {
        putchar('%');
#endif
    } else if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
        putchar('*');
    }
}
