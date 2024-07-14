#include "converters.h"
#include "find-main.h"
#include "structures.h"
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static void print_help(void)
{
    fprintf(stderr,
            "Usage: find [OPTIONS] [START_DIR]\n\n"
            "OPTIONS:\n"
            "-n NAME   Filter by substring in name\n"
            "-s SORT   Sort by full path or size\n"
            "-m MASK   Filter by permissions\n"
            "-u USER   Filter by user name\n"
            "-f NUM    Filter by minimal depth\n"
            "-t NUM    Filter by maximal depth\n"
            "-a        Go through hidden dirs\n"
            "-0        Null byte as line separator\n"
            "-h        Show help\n\n"
            "SORT:\n"
            "s         Sort by size\n"
            "f         Sort by full path\n");
}

int main(int argc, char *argv[])
{
    int status = EXIT_FAILURE;
    struct filter_options filter_opt = { 0 };
    int opt;

    while ((opt = getopt(argc, argv, "n:s:m:u:f:t:a0h")) != -1) {
        switch (opt) {
        case 'n':
            filter_opt.filter_by_name = true;
            filter_opt.name_val = optarg;
            break;
        case 'u':
            filter_opt.filter_by_user = true;

            if (filter_opt.filter_by_user && !convert_user(&filter_opt, optarg)) {
                fprintf(stderr, "user name is not valid\n");
                return status;
            }

            break;
        case 'm':
            filter_opt.filter_by_permissions = true;

            if (filter_opt.filter_by_permissions && !convert_permissions(&filter_opt, optarg)) {
                fprintf(stderr, "invalid permissions mask\n");
                return status;
            }

            break;
        case 'f':
            filter_opt.filter_by_min_depth = true;

            if (filter_opt.filter_by_min_depth && !convert_depth(optarg, &filter_opt.depth_min_val)) {
                fprintf(stderr, "invalid number\n");
                return status;
            }

            break;
        case 't':
            filter_opt.filter_by_max_depth = true;

            if (filter_opt.filter_by_max_depth && !convert_depth(optarg, &filter_opt.depth_max_val)) {
                fprintf(stderr, "invalid number\n");
                return status;
            }

            break;
        case 's':
            filter_opt.sort_override = true;

            if (filter_opt.sort_override && !convert_sort_option(&filter_opt, optarg)) {
                fprintf(stderr, "invalid sort option\n");
                return status;
            }

            break;
        case 'a':
            filter_opt.include_hidden = true;
            break;
        case '0':
            filter_opt.null_separator = true;
            break;
        case 'h':
            print_help();
            status = EXIT_SUCCESS;
            return status;
        default:
            return status;
        }
    }

    char *path;

    if (optind < argc) {
        path = argv[optind];
    } else {
        path = ".";
    }

    if (run_find(path, &filter_opt)) {
        status = EXIT_SUCCESS;
    }

    return status;
}
