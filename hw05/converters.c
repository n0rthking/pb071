#include "structures.h"
#include <limits.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static bool user_id_from_name(char *user_name, uid_t *user_id)
{
    struct passwd *pw = getpwnam(user_name);

    if (pw == NULL) {
        return false;
    }

    *user_id = pw->pw_uid;

    return true;
}

bool convert_user(struct filter_options *filter_opt, char *user_raw)
{
    uid_t user_id;

    if (!user_id_from_name(user_raw, &user_id)) {
        return false;
    }

    filter_opt->user_val = user_id;

    return true;
}

bool convert_permissions(struct filter_options *filter_opt, char *permissions_raw)
{
    char *end_ptr;
    unsigned long int converted_int = strtoul(permissions_raw, &end_ptr, 8);

    if (permissions_raw == end_ptr || *end_ptr != '\0' || converted_int == ULONG_MAX) {
        return false;
    }

    filter_opt->permissions_val = converted_int;

    return true;
}

bool convert_depth(char *depth_raw, size_t *depth)
{
    char *end_ptr;
    size_t converted_int = strtoul(depth_raw, &end_ptr, 10);

    if (depth_raw == end_ptr || *end_ptr != '\0' || converted_int == ULONG_MAX) {
        return false;
    }

    *depth = converted_int;

    return true;
}

bool convert_sort_option(struct filter_options *filter_opt, char *sort_raw)
{
    if (strlen(sort_raw) != 1) {
        return false;
    }

    switch (sort_raw[0]) {
    case 's':
        filter_opt->sort_by_size = true;
        return true;
    case 'f':
        return true;
    default:
        return false;
    }
}
