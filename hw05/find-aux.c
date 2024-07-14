#include "structures.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>

bool build_full_path(char **full_path, char *file_name, char *path, size_t *allocated)
{
    size_t total_len = strlen(file_name) + strlen(path) + 2;
    char *old_ptr = *full_path;

    if (total_len > *allocated) {
        *allocated += total_len;
        *full_path = realloc(*full_path, *allocated);
    }

    if (*full_path == NULL) {
        free(old_ptr);
        return false;
    }

    if (*(stpcpy(*full_path, path) - 1) != '/') {
        strcat(*full_path, "/");
    }

    strcat(*full_path, file_name);

    return true;
}

bool increase_capacity(struct result *res)
{
    if (res->size + 1 <= res->alloc) {
        return true;
    }

    res->alloc += 10;

    struct res_entry *ent_old = res->ent;
    res->ent = realloc(res->ent, res->alloc * sizeof(struct res_entry));

    if (res->ent == NULL) {
        free(ent_old);
        return false;
    }

    return true;
}

bool add_new_entry(struct result *res, char *file_name, char *full_path, off_t file_size)
{
    struct res_entry *curr_ent = res->ent + res->size;

    curr_ent->full_path = malloc(strlen(full_path) + 1);
    if (curr_ent->full_path == NULL) {
        return false;
    }

    strcpy(curr_ent->full_path, full_path);
    curr_ent->file_name = curr_ent->full_path + strlen(full_path) - strlen(file_name);
    curr_ent->file_size = file_size;
    res->size++;

    return true;
}

static int cmp_res_full_path(const void *a, const void *b)
{
    const struct res_entry *fst = (const struct res_entry *) a;
    const struct res_entry *snd = (const struct res_entry *) b;

    return strcmp(fst->full_path, snd->full_path);
}

static int cmp_res_file_name(const void *a, const void *b)
{
    const struct res_entry *fst = (const struct res_entry *) a;
    const struct res_entry *snd = (const struct res_entry *) b;

    int ret_val = strcasecmp(fst->file_name, snd->file_name);
    if (ret_val != 0) {
        return ret_val;
    }

    return cmp_res_full_path(a, b);
}

static int cmp_res_file_size(const void *a, const void *b)
{
    const struct res_entry *fst = (const struct res_entry *) a;
    const struct res_entry *snd = (const struct res_entry *) b;

    off_t fst_size = fst->file_size;
    off_t snd_size = snd->file_size;

    if (fst_size < snd_size) {
        return 1;
    }

    if (fst_size > snd_size) {
        return -1;
    }

    return cmp_res_file_name(a, b);
}

void sort_results(struct result *res, struct filter_options *filter_opt)
{
    int (*compare_function)(const void *, const void *);

    if (!filter_opt->sort_override) {
        compare_function = cmp_res_file_name;
    } else if (filter_opt->sort_by_size) {
        compare_function = cmp_res_file_size;
    } else {
        compare_function = cmp_res_full_path;
    }

    qsort(res->ent, res->size, sizeof(struct res_entry), compare_function);
}

void free_all_results(struct result *res)
{
    for (size_t i = 0; i < res->size; i++) {
        free(res->ent[i].full_path);
    }

    free(res->ent);
    free(res);
}

void print_results(struct filter_options *filter_opt, struct result *res)
{
    for (size_t i = 0; i < res->size; i++) {
        printf("%s", res->ent[i].full_path);
        if (filter_opt->null_separator) {
            putchar('\0');
        } else {
            putchar('\n');
        }
    }
}
