#include "find-aux.h"
#include "structures.h"
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

static bool find_rec(char *path, struct filter_options *filter_opt, struct result *res, size_t depth)
{
    bool status = true;

    if (filter_opt->filter_by_max_depth && depth >= filter_opt->depth_max_val) {
        return status;
    }

    DIR *dir = opendir(path);
    if (dir == NULL) {
        fprintf(stderr, "open failed for directory ");
        perror(path);

        if (depth == 0) {
            status = false;
        }

        return status;
    }

    struct dirent *dir_entry = NULL;
    struct stat stat_info;

    char *file_name;
    char *full_path = NULL;
    size_t allocated = 0;

    while ((dir_entry = readdir(dir)) != NULL) {
        file_name = dir_entry->d_name;
        if (!build_full_path(&full_path, file_name, path, &allocated)) {
            fprintf(stderr, "memory allocation failed\n");
            status = false;
            goto free_and_return;
        }

        if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0) {
            continue;
        }

        if (!filter_opt->include_hidden && file_name[0] == '.') {
            continue;
        }

        if (lstat(full_path, &stat_info) == -1) {
            fprintf(stderr, "stat failed for file ");
            perror(full_path);
            continue;
        }

        if (S_ISDIR(stat_info.st_mode)) {
            if (!find_rec(full_path, filter_opt, res, depth + 1)) {
                status = false;
                goto free_and_return;
            }
        }

        if (!S_ISREG(stat_info.st_mode)) {
            continue;
        }

        if (filter_opt->filter_by_name && strstr(file_name, filter_opt->name_val) == NULL) {
            continue;
        }

        if (filter_opt->filter_by_user && stat_info.st_uid != filter_opt->user_val) {
            continue;
        }

        if (filter_opt->filter_by_permissions) {
            mode_t file_permissions = stat_info.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
            if (file_permissions != filter_opt->permissions_val) {
                continue;
            }
        }

        if (filter_opt->filter_by_min_depth) {
            if (filter_opt->depth_min_val > 0 && depth < filter_opt->depth_min_val - 1) {
                continue;
            }
        }

        if (!increase_capacity(res)) {
            fprintf(stderr, "memory allocation failed\n");
            status = false;
            goto free_and_return;
        }

        if (!add_new_entry(res, file_name, full_path, stat_info.st_size)) {
            fprintf(stderr, "could not add new entry\n");
            status = false;
            goto free_and_return;
        }
    }

free_and_return:
    free(full_path);
    closedir(dir);
    return status;
}

bool run_find(char *path, struct filter_options *filter_opt)
{
    bool status = false;

    struct result *res = calloc(1, sizeof(struct result));
    if (res == NULL) {
        fprintf(stderr, "memory allocation failed\n");
        return status;
    }

    status = find_rec(path, filter_opt, res, 0);
    if (status) {
        sort_results(res, filter_opt);
        print_results(filter_opt, res);
    }

    free_all_results(res);
    return status;
}
