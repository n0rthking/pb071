#ifndef CONVERTERS_H
#define CONVERTERS_H

#include "structures.h"
#include <stdbool.h>
#include <stdlib.h>

bool convert_user(struct filter_options *filter_opt, char *user_raw);
bool convert_permissions(struct filter_options *filter_opt, char *permissions_raw);
bool convert_depth(char *depth_raw, size_t *depth);
bool convert_sort_option(struct filter_options *filter_opt, char *sort_raw);

#endif
