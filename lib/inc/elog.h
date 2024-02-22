/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015-2019, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: It is an head file for this library. You can see all be called functions.
 * Created on: 2015-04-28
 */

#ifndef __ELOG_H__
#define __ELOG_H__

#include <elog_cfg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* EasyLogger software version number */
#define ELOG_SW_VERSION "2.2.99"

/* output log's level */
#define ELOG_LVL_TOTAL_NUM        6
#define ELOG_LVL_ASSERT           0
#define ELOG_LVL_ERROR            1
#define ELOG_LVL_WARN             2
#define ELOG_LVL_INFO             3
#define ELOG_LVL_DEBUG            4
#define ELOG_LVL_VERBOSE          5

#define elog_assert(tag, ...)     elog_output(false, ELOG_LVL_ASSERT, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define elog_assert_isr(tag, ...) elog_output(true, ELOG_LVL_ASSERT, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define elog_error(tag, ...)      elog_output(false, ELOG_LVL_ERROR, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define elog_error_isr(tag, ...)  elog_output(true, ELOG_LVL_ERROR, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define elog_warn(tag, ...)       elog_output(false, ELOG_LVL_WARN, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define elog_warn_isr(tag, ...)   elog_output(true, ELOG_LVL_WARN, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define elog_info(tag, ...)       elog_output(false, ELOG_LVL_INFO, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define elog_info_isr(tag, ...)   elog_output(true, ELOG_LVL_INFO, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define elog_debug(tag, ...)      elog_output(false, ELOG_LVL_DEBUG, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define elog_debug_isr(tag, ...)  elog_output(true, ELOG_LVL_DEBUG, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define elog_verbose(tag, ...)    elog_output(false, ELOG_LVL_VERBOSE, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define elog_verbose_isr(tag, ...)                                                                                     \
    elog_output(true, ELOG_LVL_VERBOSE, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

/* easy logger */
typedef struct
{
    size_t enabled_fmt_set[ELOG_LVL_TOTAL_NUM];
    bool   init_ok;
    bool   output_enabled;
    bool   output_lock_enabled;
    bool   output_is_locked_before_enable;
    bool   output_is_locked_before_disable;

} EasyLogger, *EasyLogger_t;

/* EasyLogger error code */
typedef enum
{
    ELOG_NO_ERR,
    ELOG_INIT_FAIL,
    ELOG_INPUT_ERR,
    ELOG_NO_LOG,
} ElogErrCode;

/* elog.c */
ElogErrCode elog_init (void);
void        elog_deinit (void);
void        elog_start (void);
void        elog_stop (void);
void        elog_set_output_enabled (bool enabled);
bool        elog_get_output_enabled (void);
void        elog_set_fmt (uint8_t level, size_t set);
void   elog_output (bool is_isr, uint8_t level, const char *tag, const char *file, const char *func, const long line,
                    const char *format, ...);
void   elog_output_lock_enabled (bool enabled);
int8_t elog_find_lvl (const char *log);
const char *elog_find_tag (const char *log, uint8_t lvl, size_t *tag_len);

#define elog_a(tag, ...) elog_assert(tag, __VA_ARGS__)
#define elog_e(tag, ...) elog_error(tag, __VA_ARGS__)
#define elog_w(tag, ...) elog_warn(tag, __VA_ARGS__)
#define elog_i(tag, ...) elog_info(tag, __VA_ARGS__)
#define elog_d(tag, ...) elog_debug(tag, __VA_ARGS__)
#define elog_v(tag, ...) elog_verbose(tag, __VA_ARGS__)

// API for interrupt context
#define elog_a_isr(tag, ...) elog_assert_isr(tag, __VA_ARGS__)
#define elog_e_isr(tag, ...) elog_error_isr(tag, __VA_ARGS__)
#define elog_w_isr(tag, ...) elog_warn_isr(tag, __VA_ARGS__)
#define elog_i_isr(tag, ...) elog_info_isr(tag, __VA_ARGS__)
#define elog_d_isr(tag, ...) elog_debug_isr(tag, __VA_ARGS__)
#define elog_v_isr(tag, ...) elog_verbose_isr(tag, __VA_ARGS__)

/* elog_async.c */
void        elog_async_enabled (bool enabled);
ElogErrCode elog_async_get_line_log (char *log, size_t size);

// 64bit timestamp
typedef struct
{
    uint32_t low;
    uint32_t high;
} elog_timestamp_t;

typedef struct
{
    uint32_t         seq_num;
    uint8_t          level;
    elog_timestamp_t timestamp;
    uint32_t         message_length;
} elog_header_t;

#endif