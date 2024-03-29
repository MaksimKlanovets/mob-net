/**
 * @file common.h
 * @author Radek Krejci <rkrejci@cesnet.cz>
 * @brief common internal definitions for libyang
 *
 * Copyright (c) 2015 - 2018 CESNET, z.s.p.o.
 *
 * This source code is licensed under BSD 3-Clause License (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://opensource.org/licenses/BSD-3-Clause
 */

#ifndef LY_COMMON_H_
#define LY_COMMON_H_

#include <stdint.h>
#include <errno.h>
#include <inttypes.h>

#include "libyangextension/headers/compat.h"
#include "libyang/libyang.h"
#include "libyangextension/headers/hash_table.h"
#include "libyangextension/headers/resolve.h"

#if __STDC_VERSION__ >= 201112 && \
    !defined __STDC_NO_THREADS__ && \
    !defined __NetBSD__
# define THREAD_LOCAL _Thread_local
#elif defined __GNUC__ || \
      defined __SUNPRO_C || \
      defined __xlC__
# define THREAD_LOCAL __thread
#else
# error "Cannot define THREAD_LOCAL"
#endif

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))

#define LY_CHECK_GOTO(COND, GOTO) if (COND) {goto GOTO;}
#define LY_CHECK_ERR_GOTO(COND, ERR, GOTO) if (COND) {ERR; goto GOTO;}
#define LY_CHECK_RETURN(COND, RETVAL) if (COND) {return RETVAL;}
#define LY_CHECK_ERR_RETURN(COND, ERR, RETVAL) if (COND) {ERR; return RETVAL;}

/*
 * If the compiler supports attribute to mark objects as hidden, mark all
 * objects as hidden and export only objects explicitly marked to be part of
 * the public API.
 */
#define API __attribute__((visibility("default")))

/* how many bytes add when enlarging buffers */
#define LY_BUF_STEP 128

/* internal logging options */
enum int_log_opts {
    ILO_LOG = 0, /* log normally */
    ILO_STORE,   /* only store any messages, they will be processed higher on stack */
    ILO_IGNORE,  /* completely ignore messages */
    ILO_ERR2WRN, /* change errors to warnings */
};

void ly_err_free(void *ptr);
void ly_err_free_next(struct ly_ctx *ctx, struct ly_err_item *last_eitem);
void ly_ilo_change(struct ly_ctx *ctx, enum int_log_opts new_ilo, enum int_log_opts *prev_ilo, struct ly_err_item **prev_last_eitem);
void ly_ilo_restore(struct ly_ctx *ctx, enum int_log_opts prev_ilo, struct ly_err_item *prev_last_eitem, int keep_and_print);
void ly_err_last_set_apptag(const struct ly_ctx *ctx, const char *apptag);
void ly_err_last_set_msg(const struct ly_ctx *ctx, const char *msg);
extern THREAD_LOCAL enum int_log_opts log_opt;

/*
 * logger
 */
extern volatile uint8_t ly_log_level;
extern volatile uint8_t ly_log_opts;
extern volatile int ly_log_dbg_groups;

void ly_log(const struct ly_ctx *ctx, LY_LOG_LEVEL level, LY_ERR no, const char *format, ...);

#define LOGERR(ctx, errno, str, args...)                            \
    ly_log(ctx, LY_LLERR, errno, str, ##args);

#define LOGWRN(ctx, str, args...)                                   \
    ly_log(ctx, LY_LLWRN, 0, str, ##args);

#define LOGVRB(str, args...)                                        \
    ly_log(NULL, LY_LLVRB, 0, str, ##args);

#ifdef NDEBUG

#define LOGDBG(dbg_group, str, args...)

#else

#define LOGDBG(dbg_group, str, args...)                             \
    ly_log_dbg(dbg_group, str, ##args);

void ly_log_dbg(int group, const char *format, ...);

#endif

#define FUN_IN LOGDBG(LY_LDGAPI, "%s", __func__);

#define LOGMEM(ctx) LOGERR(ctx, LY_EMEM, "Memory allocation failed (%s()).", __func__)

#define LOGINT(ctx) LOGERR(ctx, LY_EINT, "Internal error (%s:%d).", __FILE__, __LINE__)

#define LOGARG LOGERR(NULL, LY_EINVAL, "Invalid arguments (%s()).", __func__)

typedef enum {
    LYE_PATH = -2,    /**< error path set */
    LYE_SPEC = -1,    /**< generic error */

    LYE_SUCCESS = 0,

    LYE_XML_MISS,
    LYE_XML_INVAL,
    LYE_XML_INCHAR,

    LYE_EOF,
    LYE_INSTMT,
    LYE_INCHILDSTMT,
    LYE_INPAR,
    LYE_INID,
    LYE_INDATE,
    LYE_INARG,
    LYE_MISSSTMT,
    LYE_MISSCHILDSTMT,
    LYE_MISSARG,
    LYE_TOOMANY,
    LYE_DUPID,
    LYE_DUPLEAFLIST,
    LYE_DUPLIST,
    LYE_NOUNIQ,
    LYE_ENUM_INVAL,
    LYE_ENUM_INNAME,
    LYE_ENUM_DUPVAL,
    LYE_ENUM_DUPNAME,
    LYE_ENUM_WS,
    LYE_BITS_INVAL,
    LYE_BITS_INNAME,
    LYE_BITS_DUPVAL,
    LYE_BITS_DUPNAME,
    LYE_INMOD,
    LYE_INMOD_LEN,
    LYE_KEY_NLEAF,
    LYE_KEY_TYPE,
    LYE_KEY_CONFIG,
    LYE_KEY_MISS,
    LYE_KEY_DUP,
    LYE_INREGEX,
    LYE_INRESOLV,
    LYE_INSTATUS,
    LYE_CIRC_LEAFREFS,
    LYE_CIRC_FEATURES,
    LYE_CIRC_IMPORTS,
    LYE_CIRC_INCLUDES,
    LYE_INVER,
    LYE_SUBMODULE,

    LYE_OBSDATA,
    LYE_OBSTYPE,
    LYE_NORESOLV,
    LYE_INELEM,
    LYE_INELEM_LEN,
    LYE_MISSELEM,
    LYE_INVAL,
    LYE_INMETA,
    LYE_INATTR,
    LYE_MISSATTR,
    LYE_NOCONSTR,
    LYE_INCHAR,
    LYE_INPRED,
    LYE_MCASEDATA,
    LYE_NOMUST,
    LYE_NOWHEN,
    LYE_INORDER,
    LYE_INWHEN,
    LYE_NOMIN,
    LYE_NOMAX,
    LYE_NOREQINS,
    LYE_NOLEAFREF,
    LYE_NOMANDCHOICE,

    LYE_XPATH_INTOK,
    LYE_XPATH_EOF,
    LYE_XPATH_INOP_1,
    LYE_XPATH_INOP_2,
    LYE_XPATH_INCTX,
    LYE_XPATH_INMOD,
    LYE_XPATH_INFUNC,
    LYE_XPATH_INARGCOUNT,
    LYE_XPATH_INARGTYPE,
    LYE_XPATH_DUMMY,
    LYE_XPATH_NOEND,

    LYE_PATH_INCHAR,
    LYE_PATH_INMOD,
    LYE_PATH_MISSMOD,
    LYE_PATH_INNODE,
    LYE_PATH_INKEY,
    LYE_PATH_MISSKEY,
    LYE_PATH_INIDENTREF,
    LYE_PATH_EXISTS,
    LYE_PATH_MISSPAR,
    LYE_PATH_PREDTOOMANY,
} LY_ECODE;

enum LY_VLOG_ELEM {
    LY_VLOG_NONE = 0,
    LY_VLOG_XML, /* struct lyxml_elem* */
    LY_VLOG_LYS, /* struct lys_node* */
    LY_VLOG_LYD, /* struct lyd_node* */
    LY_VLOG_STR, /* const char* */
    LY_VLOG_PREV /* use exact same previous path */
};

void ly_vlog(const struct ly_ctx *ctx, LY_ECODE code, enum LY_VLOG_ELEM elem_type, const void *elem, ...);
#define LOGVAL(ctx, code, elem_type, elem, args...)                      \
    ly_vlog(ctx, code, elem_type, elem, ##args);

#define LOGPATH(ctx, elem_type, elem)                                    \
    ly_vlog(ctx, LYE_PATH, elem_type, elem);

/**
 * @brief Print additional validation information string.
 *
 * All special characters will be escaped ('%').
 *
 * @param[in] ctx Context to use for logging.
 * @param[in] elem_type Identify the element at issue. Either #LY_VLOG_NONE or #LY_VLOG_PREV.
 * @param[in] str String to print that will be escaped.
 * @param[in] ... Always leave empty (needed for compiler to accept va_start() call).
 */
void ly_vlog_str(const struct ly_ctx *ctx, enum LY_VLOG_ELEM elem_type, const char *str, ...);

/**
 * @brief Build path of \p elem.
 *
 * @param[in] elem_type What to expect in \p elem.
 * @param[in] elem Element to print.
 * @param[in,out] path Resulting path printed.
 * @param[in] schema_all_prefixes Whether to include prefixes for all the nodes (only for schema paths).
 * @param[in] data_no_last_predicate Whether to skip generating predicate for the last node (only for data paths).
 * @return 0 on success, -1 on error.
 */
int ly_vlog_build_path(enum LY_VLOG_ELEM elem_type, const void *elem, char **path, int schema_all_prefixes, int data_no_last_predicate);

/**
 * @brief Get module from a context based on its name and revision.
 *
 * @param[in] ctx Context to search in.
 * @param[in] name Name of the module.
 * @param[in] name_len Length of \p name, can be 0 if the name is ended with '\0'.
 * @param[in] revision Revision of the module, can be NULL for the newest.
 * @param[in] implemented Whether only implemented modules should be returned.
 * @return Matching module, NULL if not found.
 */
const struct lys_module *ly_ctx_nget_module(const struct ly_ctx *ctx, const char *name, size_t name_len,
                                            const char *revision, int implemented);

/*
 * - if \p module specified, it searches for submodules, they can be loaded only from a file or via module callback,
 *   they cannot be get from context
 * - if \p module is not specified
 *   - if specific revision is specified, the first try is to get module from the context
 *   - if no specific revision is specified, it tries to get the newest module - first it searches for the file and
 *     then checks that the schema loaded from the same source isn't already in context. If the source wasn't
 *     previously loaded, it is parsed.
 */
const struct lys_module *ly_ctx_load_sub_module(struct ly_ctx *ctx, struct lys_module *module, const char *name,
                                                const char *revision, int implement, struct unres_schema *unres);

/**
 * @brief Basic functionality like strpbrk(3). However, it searches string \p s
 *        backwards up to most \p s_len characters.
 *
 * @param[in] s String to search backwards.
 * @param[in] accept String of characters that are searched for.
 * @param[in] s_len Backward length of \p s.
 *
 * @return Pointer to the first backward occurrence of a character from
 *         \p accept or \p s - \p s_len if not found.
 */
const char *strpbrk_backwards(const char *s, const char *accept, unsigned int s_len);

char *strnchr(const char *s, int c, unsigned int len);

const char *strnodetype(LYS_NODE type);

/**
 * @brief Transform a module name (JSON format prefix) to a prefix as defined
 * in \p module imports. Its own name is transformed to its own prefix.
 *
 * @param[in] module Module with imports to use.
 * @param[in] module_name Module name to transform.
 *
 * @return Module import prefix (do not modify, free, or lydict_remove),
 * NULL on error.
 */
const char *transform_module_name2import_prefix(const struct lys_module *module, const char *module_name);

/**
 * @brief Transform expression from JSON format to XML format.
 * Output arrays point to strings in the dictionary, but without
 * correcting their ref_count -> do not touch them. Prefixes of
 * the namespaces are prefixes specified by the module itself. Output
 * parameters are optional, but either all 3 are set or none
 * of them are. Logs directly.
 *
 * @param[in] module Module with imports to use.
 * @param[in] expr JSON expression.
 * @param[in] inst_id Whether to add prefixes to all node names (XML instance-identifier).
 * @param[out] prefixes Array of pointers to prefixes. After use free them with free(*prefixes).
 * Can be NULL.
 * @param[out] namespaces Array of pointers to full namespaces. After use free them with
 * free(*namespaces). Can be NULL.
 * @param[out] ns_count Number of elements in both \p prefixes and \p namespaces arrays.
 * Can be NULL.
 *
 * @return Transformed XML expression in the dictionary, NULL on error.
 */
const char *transform_json2xml(const struct lys_module *module, const char *expr, int inst_id, const char ***prefixes,
                               const char ***namespaces, uint32_t *ns_count);

/**
 * @brief Transform expression from JSON format to schema format.
 * Prefixes of the namespaces (models) are those from the main
 * \p module imports of the corresponding modules. Logs directly.
 *
 * @param[in] module Module with imports to use.
 * @param[in] expr JSON expression.
 *
 * @return Transformed XML expression in the dictionary, NULL on error.
 */
const char *transform_json2schema(const struct lys_module *module, const char *expr);

/**
 * @brief Transform expression from XML data format (prefixes and separate NS definitions) to
 *        JSON format (prefixes are module names instead). Logs directly.
 *
 * @param[in] ctx libyang context to use.
 * @param[in] expr XML expression.
 * @param[in] xml XML element with the expression.
 * @param[in] inst_id Whether all the node names must have a prefix (XML instance-identifier).
 * @param[in] use_ctx_data_clb Whether to use data_clb in \p ctx if an unknown module namespace is found.
 *
 * @return Transformed JSON expression in the dictionary, NULL on error.
 */
const char *transform_xml2json(struct ly_ctx *ctx, const char *expr, struct lyxml_elem *xml, int inst_id,
                               int use_ctx_data_clb);

/**
 * @brief Transform expression from the schema format (prefixes of imports) to
 *        JSON format (prefixes are module names directly). Logs directly.
 *
 * @param[in] module Module (schema) with imports to search.
 * @param[in] expr Expression from \p module.
 *
 * @return Transformed JSON expression in the dictionary, NULL on error.
 */
const char *transform_schema2json(const struct lys_module *module, const char *expr);

/**
 * @brief Same as transform_schema2json, but dumbed down, because if-feature expressions
 *        are not valid XPath expressions.
 */
const char *transform_iffeat_schema2json(const struct lys_module *module, const char *expr);

/**
 * @brief Transform an XPath expression in JSON node naming conventions into
 *        standard YANG XPath.
 */
char *transform_json2xpath(const struct lys_module *cur_module, const char *expr);

/**
 * @brief Get a new node (non-validated) validity value.
 *
 * @param[in] schema Schema node of the new data node.
 *
 * @return Validity of the new node.
 */
int ly_new_node_validity(const struct lys_node *schema);

/**
 * @brief Wrapper for realloc() call. The only difference is that if it fails to
 * allocate the requested memory, the original memory is freed as well.
 *
 * @param[in] ptr Memory to reallocate.
 * @param[in] size New size of the memory block.
 *
 * @return Pointer to the new memory, NULL on error.
 */
void *ly_realloc(void *ptr, size_t size);

/**
 * @brief Compare strings
 * @param[in] s1 First string to compare
 * @param[in] s2 Second string to compare
 * @param[in] both_in_dictionary Flag for optimization, 1 if it is sure that \p s1 and \p s2 were stored in dictionary.
 * This parameter is supposed to be a number (digit) known in compile time, not a variable or expression!
 * @return 1 if both strings are the same, 0 if they differ.
 */
int ly_strequal_(const char *s1, const char *s2);
#define ly_strequal0(s1, s2) ly_strequal_(s1, s2)
#define ly_strequal1(s1, s2) (s1 == s2)
#define ly_strequal(s1, s2, d) ly_strequal##d(s1, s2)

int64_t dec_pow(uint8_t exp);

int dec64cmp(int64_t num1, uint8_t dig1, int64_t num2, uint8_t dig2);

/**
 * @brief Get number of characters in the @p str, taking multibyte characters into account.
 * @param[in] str String to examine.
 * @return Number of characters in (possibly) multibyte characters string.
 */
size_t ly_strlen_utf8(const char *str);

#endif /* LY_COMMON_H_ */
