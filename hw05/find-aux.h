#ifndef FIND_AUX_H
#define FIND_AUX_H

#include "structures.h"
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>

bool build_full_path(char **full_path, char *file_name, char *path, size_t *allocated);
bool increase_capacity(struct result *res);
bool add_new_entry(struct result *res, char *file_name, char *full_path, off_t file_size);
void sort_results(struct result *res, struct filter_options *filter_opt);
void free_all_results(struct result *res);
void print_results(struct filter_options *filter_opt, struct result *res);

#endif
