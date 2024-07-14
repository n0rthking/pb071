#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>

struct filter_options
{
    bool sort_override;
    bool sort_by_size;
    bool null_separator;
    bool include_hidden;
    bool filter_by_name;
    bool filter_by_user;
    bool filter_by_permissions;
    bool filter_by_min_depth;
    bool filter_by_max_depth;
    char *name_val;
    uid_t user_val;
    unsigned long int permissions_val;
    size_t depth_min_val;
    size_t depth_max_val;
};

struct res_entry
{
    char *file_name;
    char *full_path;
    off_t file_size;
};

struct result
{
    size_t alloc;
    size_t size;
    struct res_entry *ent;
};

#endif
