#include "list.h"
#include "format.h"
#include "util.h"

#include <dirent.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static Options *g_opts = NULL;

int g_exit_code = 0;

static int cmp_lex(const void *a, const void *b) {
    const FileInfo *fa = (const FileInfo *)a;
    const FileInfo *fb = (const FileInfo *)b;
    return strcmp(fa->name, fb->name);
}

static time_t file_time(const FileInfo *fi, const Options *opts) {
    switch (opts->time_mode) {
    case TIME_ATIME:
        return fi->st.st_atime;
    case TIME_CTIME:
        return fi->st.st_ctime;
    default:
        return fi->st.st_mtime;
    }
}

static int cmp_fileinfo(const void *a, const void *b) {
    const FileInfo *fa = (const FileInfo *)a;
    const FileInfo *fb = (const FileInfo *)b;
    int res = 0;

    if (!g_opts->no_sort) {
        if (g_opts->sort_size) {
            if (fa->st.st_size > fb->st.st_size) {
                res = -1;
            } else if (fa->st.st_size < fb->st.st_size) {
                res = 1;
            }
        } else if (g_opts->sort_time) {
            time_t ta = file_time(fa, g_opts);
            time_t tb = file_time(fb, g_opts);
            if (ta > tb) {
                res = -1;
            } else if (ta < tb) {
                res = 1;
            }
        }

        if (res == 0) {
            res = strcmp(fa->name, fb->name);
        }
    }

    if (g_opts->reverse) {
        res = -res;
    }

    return res;
}

void fl_init(FileList *fl) {
    fl->items = NULL;
    fl->len = 0;
    fl->cap = 0;
}

void fl_add(FileList *fl, const char *name, const char *path,
            struct stat *st, bool stat_ok) {
    if (fl->len == fl->cap) {
        size_t newcap = fl->cap ? fl->cap * 2 : 32;
        FileInfo *newitems = realloc(fl->items,
                                     newcap * sizeof(FileInfo));
        if (!newitems) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        fl->items = newitems;
        fl->cap = newcap;
    }

    FileInfo *fi = &fl->items[fl->len++];
    fi->name = xstrdup(name);
    fi->path = xstrdup(path);

    if (st) {
        fi->st = *st;
    } else {
        memset(&fi->st, 0, sizeof(fi->st));
    }

    fi->stat_ok = stat_ok;
}

void fl_free(FileList *fl) {
    for (size_t i = 0; i < fl->len; i++) {
        free(fl->items[i].name);
        free(fl->items[i].path);
    }
    free(fl->items);
    fl_init(fl);
}

static void print_entries(FileList *fl, Options *opts,
                          bool print_total_line, bool do_sort) {
    if (fl->len == 0) {
        return;
    }

    if (do_sort && !opts->no_sort && fl->len > 1) {
        g_opts = opts;
        qsort(fl->items, fl->len, sizeof(FileInfo), cmp_fileinfo);
    }

    if (print_total_line && (opts->long_format || opts->blocks)) {
        long total = 0;
        for (size_t i = 0; i < fl->len; i++) {
            if (fl->items[i].stat_ok) {
                total += fl->items[i].st.st_blocks;
            }
        }
        print_total(total, opts);
    }

    int w_links = 0;
    int w_owner = 0;
    int w_group = 0;
    int w_size = 0;

    if (opts->long_format) {
        for (size_t i = 0; i < fl->len; i++) {
            FileInfo *fi = &fl->items[i];
            if (!fi->stat_ok) {
                continue;
            }

            char buf[64];
            int len = snprintf(NULL, 0, "%ju",
                               (uintmax_t)fi->st.st_nlink);
            if (len > w_links) {
                w_links = len;
            }

            format_owner(fi->st.st_uid, opts->numeric,
                         buf, sizeof(buf));
            len = (int)strlen(buf);
            if (len > w_owner) {
                w_owner = len;
            }

            format_group(fi->st.st_gid, opts->numeric,
                         buf, sizeof(buf));
            len = (int)strlen(buf);
            if (len > w_group) {
                w_group = len;
            }

            format_size(fi, opts, buf, sizeof(buf));
            len = (int)strlen(buf);
            if (len > w_size) {
                w_size = len;
            }
        }
    }

    for (size_t i = 0; i < fl->len; i++) {
        FileInfo *fi = &fl->items[i];

        if (!fi->stat_ok) {
            print_name(fi->name, opts);
            putchar('\n');
            continue;
        }

        if (opts->long_format) {
            char mode[11];
            char owner[64];
            char group[64];
            char sizebuf[64];
            char timebuf[64];

            format_mode(fi->st.st_mode, mode);
            format_owner(fi->st.st_uid, opts->numeric,
                         owner, sizeof(owner));
            format_group(fi->st.st_gid, opts->numeric,
                         group, sizeof(group));
            format_size(fi, opts, sizebuf, sizeof(sizebuf));
            format_time(fi, opts, timebuf, sizeof(timebuf));

            if (opts->inode) {
                printf("%ju ", (uintmax_t)fi->st.st_ino);
            }
            if (opts->blocks) {
                char blockbuf[64];
                format_blocks(fi, opts, blockbuf, sizeof(blockbuf));
                printf("%s ", blockbuf);
            }

            printf("%s %*ju %-*s %-*s %*s %s ",
                   mode,
                   w_links, (uintmax_t)fi->st.st_nlink,
                   w_owner, owner,
                   w_group, group,
                   w_size, sizebuf,
                   timebuf);

            print_name(fi->name, opts);

            if (S_ISLNK(fi->st.st_mode)) {
                char target[PATH_MAX];
                ssize_t n = readlink(fi->path, target,
                                     sizeof(target) - 1);
                if (n >= 0) {
                    target[n] = '\0';
                    printf(" -> ");
                    print_name(target, opts);
                }
            }

            if (opts->indicator) {
                print_indicator(fi->st.st_mode);
            }
            putchar('\n');
        } else {
            if (opts->inode) {
                printf("%ju ", (uintmax_t)fi->st.st_ino);
            }
            if (opts->blocks) {
                char blockbuf[64];
                format_blocks(fi, opts, blockbuf, sizeof(blockbuf));
                printf("%s ", blockbuf);
            }

            print_name(fi->name, opts);

            if (opts->indicator) {
                print_indicator(fi->st.st_mode);
            }
            putchar('\n');
        }
    }
}

static void list_dir(const char *path, Options *opts,
                     bool print_header) {
    DIR *dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "ls: %s: %s\n", path, strerror(errno));
	g_exit_code = 1;
        return;
    }

    if (print_header) {
        printf("%s:\n", path);
    }

    FileList fl;
    fl_init(&fl);

    struct dirent *de;
    while ((de = readdir(dir)) != NULL) {
        const char *name = de->d_name;

        if (!opts->show_all && !opts->show_almost_all &&
            name[0] == '.') {
            continue;
        }

        if (opts->show_almost_all &&
            (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)) {
            continue;
        }

        char *full = join_path(path, name);
        struct stat st;
        bool ok = (lstat(full, &st) == 0);

        fl_add(&fl, name, full, &st, ok);
        free(full);
    }

    closedir(dir);

    print_entries(&fl, opts, true, true);

    if (opts->recursive) {
        for (size_t i = 0; i < fl.len; i++) {
            FileInfo *fi = &fl.items[i];
            if (fi->stat_ok && S_ISDIR(fi->st.st_mode) &&
                strcmp(fi->name, ".") != 0 &&
                strcmp(fi->name, "..") != 0) {
                list_dir(fi->path, opts, true);
            }
        }
    }

    fl_free(&fl);
}

void list_main(Options *opts, int num_operands, char **operands) {
    FileList files;
    FileList dirs;

    fl_init(&files);
    fl_init(&dirs);

    if (num_operands == 0) {
        list_dir(".", opts, false);
        return;
    }

    for (int i = 0; i < num_operands; i++) {
        const char *path = operands[i];
        struct stat st;

        if (opts->directory_plain) {
            if (lstat(path, &st) == 0) {
                fl_add(&files, path, path, &st, true);
            } else {
                fprintf(stderr, "ls: %s: %s\n",
                        path, strerror(errno));
		g_exit_code = 1;        
    	    }
        } else {
            if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
                fl_add(&dirs, path, path, &st, true);
            } else {
                struct stat lst;
                if (lstat(path, &lst) == 0) {
                    fl_add(&files, path, path, &lst, true);
                } else {
                    fprintf(stderr, "ls: %s: %s\n",
                            path, strerror(errno));
		    g_exit_code = 1;                
		}
            }
        }
    }

    if (files.len > 1) {
        qsort(files.items, files.len, sizeof(FileInfo), cmp_lex);
    }
    if (dirs.len > 1) {
        qsort(dirs.items, dirs.len, sizeof(FileInfo), cmp_lex);
    }

    if (files.len > 0) {
        print_entries(&files, opts, false, false);
    }

    for (size_t i = 0; i < dirs.len; i++) {
        bool header = (files.len + dirs.len > 1) || opts->recursive;
        list_dir(dirs.items[i].path, opts, header);
    }

    fl_free(&files);
    fl_free(&dirs);
}
