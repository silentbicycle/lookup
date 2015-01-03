/* 
 * Copyright (c) 2014 Scott Vokes <vokes.s@gmail.com>
 *  
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include <err.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define LOOKUP_VERSION_MAJOR 0
#define LOOKUP_VERSION_MINOR 1
#define LOOKUP_VERSION_PATCH 0
#define LOOKUP_AUTHOR "Scott Vokes <vokes.s@gmail.com>"

typedef struct {
    const char *name;
    bool relative;
    bool parent_only;
} config;

static bool exists(char *path);
static int print_path_name(config *cfg, char *path_buf);

static char buf[PATH_MAX];

static void usage(void) {
    printf("lookup version %d.%d.%d by %s\n",
        LOOKUP_VERSION_MAJOR, LOOKUP_VERSION_MINOR, LOOKUP_VERSION_PATCH,
        LOOKUP_AUTHOR);
    printf("Usage: lookup [-d] [-r] FILENAME\n"
        "  -p    parent directory only (not filename)\n"
        "  -r    relative path\n");
    exit(1);
}

static bool proc_args(config *cfg, int argc, char **argv) {
    int a = 0;
    while ((a = getopt(argc, argv, "pr")) != -1) {
        switch (a) {
        case 'p':           /* parent directory only */
            cfg->parent_only = true;
            break;
        case 'r':           /* relative path */
            cfg->relative = true;
            break;
        case '?':
        default:
            fprintf(stderr, "lookup: illegal option: -- %c\n", a);
            usage();
        }
    }
    
    argc -= optind;
    argv += optind;
    if (argc < 1) { usage(); }
    cfg->name = argv[0];
    return true;
}

static int run(config *cfg) {
    size_t offset = 0;

    struct stat fs;
    ino_t last = (ino_t)0;

    for (;;) {
        int rem = PATH_MAX - offset;
        if (rem < snprintf(&buf[offset], rem, "%s", cfg->name)) {
            fprintf(stderr, "PATH_MAX exceeded\n");
            return 1;
        }
        if (exists(buf)) {
            if (cfg->parent_only) { buf[offset] = '\0'; }
            return print_path_name(cfg, buf);
        }

        if (rem < snprintf(&buf[offset], rem, "../")) {
            fprintf(stderr, "PATH_MAX exceeded\n");
            return 1;
        }
        if (stat(buf, &fs) == -1) {
            printf("%s\n", strerror(errno));
            break;
        }

        /* Check for inode cycle, e.g. at / or symlink */
        if (fs.st_ino == last) { break; }

        offset += strlen("../");
        last = fs.st_ino;
    }
    return 1;
}

int main(int argc, char **argv) {
    config cfg;
    memset(&cfg, 0, sizeof(cfg));
    if (!proc_args(&cfg, argc, argv)) { usage(); }
    return run(&cfg);
}

static bool exists(char *path) {
    struct stat fs;
    if (stat(path, &fs) == -1) {
        if (errno == ENOENT) {
            errno = 0;
            return false;
        } else {
            err(1, "stat");
        }
    } else {
        return true;
    }    
}

static int print_path_name(config *cfg, char *path_buf) {
    if (path_buf[0] == '\0') {
        path_buf[0] = '.';
        path_buf[1] = '\0';
    }
    if (cfg->relative) {
        printf("%s\n", path_buf);
    } else {
        char unused[PATH_MAX];
        char *rp = realpath(path_buf, unused);
        if (rp == NULL) { err(1, "realpath"); }
        printf("%s\n", rp);
    }
    return 0;
}
