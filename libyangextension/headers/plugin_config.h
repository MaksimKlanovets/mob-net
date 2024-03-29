/**
 * @file plugin_config.h
 * @author Michal Vasko <mvasko@cesnet.cz>
 * @brief libyang plugin config file
 *
 * Copyright (c) 2018 CESNET, z.s.p.o.
 *
 * This source code is licensed under BSD 3-Clause License (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://opensource.org/licenses/BSD-3-Clause
 */

#ifndef LY_PLUGIN_CONFIG_H_
#define LY_PLUGIN_CONFIG_H_

#define LYEXT_PLUGINS_DIR "/usr/local/lib/libyang1/extensions" /**< directory with YANG extension plugins */
#define LY_USER_TYPES_PLUGINS_DIR "/usr/local/lib/libyang1/user_types" /**< directory with user YANG types plugins */

#if defined __linux__ || defined __unix__
#  define LY_PLUGIN_SUFFIX ".so"
#  define LY_PLUGIN_SUFFIX_LEN 3
#elif defined __APPLE__
#  define LY_PLUGIN_SUFFIX ".dylib"
#  define LY_PLUGIN_SUFFIX_LEN 6
#endif

#ifdef STATIC



int lytype_size(struct lytype_plugin_list plugin[]) {
    struct lytype_plugin_list *tmp;
    int i = 0;
    while (1) {
        tmp = &plugin[i];
        if (!tmp) break;
        if (!tmp->module && !tmp->revision && !tmp->name && !tmp->store_clb && !tmp->free_clb) break;
        i++;
    }

    return i;
}

void lytype_add(struct lytype_plugin_list *type_plugins, uint16_t *type_plugins_count, struct lytype_plugin_list plugin[]) {
    struct lytype_plugin_list *tmp;
    int i = 0;
    while (1) {
        tmp = &plugin[i];
        if (!tmp) break;
        if (!tmp->module && !tmp->revision && !tmp->name && !tmp->store_clb && !tmp->free_clb) break;
        i++;
        memcpy(&type_plugins[*type_plugins_count], tmp, sizeof *tmp);
        (*type_plugins_count)++;
    }

    return;
}


int lyext_size(struct lyext_plugin_list plugin[]) {
    struct lyext_plugin_list *tmp;
    int i = 0;
    while (1) {
        tmp = &plugin[i];
        if (!tmp) break;
        if (!tmp->module && !tmp->revision && !tmp->name && !tmp->plugin) break;
        i++;
    }

    return i;
}

void lyext_add(struct lyext_plugin_list *ext_plugins, uint16_t *ext_plugins_count, struct lyext_plugin_list plugin[]) {
    struct lyext_plugin_list *tmp;
    int i = 0;
    while (1) {
        tmp = &plugin[i];
        if (!tmp) break;
        if (!tmp->module && !tmp->revision && !tmp->name && !tmp->plugin) break;
        i++;
        memcpy(&ext_plugins[*ext_plugins_count], tmp, sizeof *tmp);
        (*ext_plugins_count)++;
    }

    return;
}

struct lyext_plugin_list *static_load_lyext_plugins(uint16_t *count)
{
    struct lyext_plugin_list *plugin;
    uint16_t size = ;

    plugin = malloc(size * (sizeof *plugin));
    if (!plugin) {
        LOGMEM(NULL);
        return NULL;
    }

    *count = 0;

    return plugin;
}

struct lytype_plugin_list *static_load_lytype_plugins(uint16_t *count)
{
    struct lytype_plugin_list *plugin;
    uint16_t size = ;

    plugin = malloc(size * (sizeof *plugin));
    if (!plugin) {
        LOGMEM(NULL);
        return NULL;
    }

    *count = 0;

    return plugin;
}

uint16_t static_loaded_plugins_count = ;
char *static_loaded_plugins[] = {};

#endif /* STATIC */

#endif /* LY_PLUGIN_CONFIG_H_ */
