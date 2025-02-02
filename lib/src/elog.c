/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015-2018, Armink, <armink.ztl@gmail.com>
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
 * Function: Initialize function and other general function.
 * Created on: 2015-04-28
 */

#define LOG_TAG            "elog"
#define ELOG_LINE_BUF_SIZE 1024

#include <elog.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

#if !defined(ELOG_NEWLINE_SIGN)
    #error "Please configure output newline sign (in elog_cfg.h)"
#endif

/* EasyLogger object */
static EasyLogger elog;
/* every line log's buffer */
static char line_log_buf[ELOG_LINE_BUF_SIZE] = {0};
/* The sequence number of the message */
static uint32_t g_seq_num = 0;
/* level output info */
const char *level_output_info[] = {
    [ELOG_LVL_ASSERT]  = "[Assert]",
    [ELOG_LVL_ERROR]   = "[Error]",
    [ELOG_LVL_WARN]    = "[Warn]",
    [ELOG_LVL_INFO]    = "[Info]",
    [ELOG_LVL_DEBUG]   = "[Debug]",
    [ELOG_LVL_VERBOSE] = "[Verbose]",
};

extern void elog_port_output (const char *log, size_t size);
extern bool elog_port_output_lock (void);
extern bool elog_port_output_unlock (void);
extern bool elog_port_output_lock_isr (void);
extern bool elog_port_output_unlock_isr (void);

/**
 * EasyLogger initialize.
 *
 * @return result
 */
ElogErrCode elog_init (void)
{
    extern ElogErrCode elog_port_init(void);
    extern ElogErrCode elog_async_init(void);

    ElogErrCode result = ELOG_NO_ERR;

    if (elog.init_ok == true)
    {
        return result;
    }

    /* port initialize */
    result = elog_port_init();
    if (result != ELOG_NO_ERR)
    {
        return result;
    }

    /* enable the output lock */
    elog_output_lock_enabled(true);
    /* output locked status initialize */
    elog.output_is_locked_before_enable  = false;
    elog.output_is_locked_before_disable = false;

    elog.init_ok = true;

    return result;
}

/**
 * EasyLogger deinitialize.
 *
 */
void elog_deinit (void)
{
    extern ElogErrCode elog_port_deinit(void);
    extern ElogErrCode elog_async_deinit(void);

    if (!elog.init_ok)
    {
        return;
    }

    /* port deinitialize */
    elog_port_deinit();

    elog.init_ok = false;
}

/**
 * EasyLogger start after initialize.
 */
void elog_start (void)
{
    if (!elog.init_ok)
    {
        return;
    }

    /* enable output */
    elog_set_output_enabled(true);

    elog_async_enabled(true);

    /* show version */
    elog_i("EasyLogger V%s is initialize success.", ELOG_SW_VERSION);
}

/**
 * EasyLogger stop after initialize.
 */
void elog_stop (void)
{
    if (!elog.init_ok)
    {
        return;
    }

    /* disable output */
    elog_set_output_enabled(false);

    elog_async_enabled(false);

    /* show version */
    elog_i("EasyLogger V%s is deinitialize success.", ELOG_SW_VERSION);
}

/**
 * set output enable or disable
 *
 * @param enabled TRUE: enable FALSE: disable
 */
void elog_set_output_enabled (bool enabled)
{
    elog.output_enabled = enabled;
}

/**
 * get output is enable or disable
 *
 * @return enable or disable
 */
bool elog_get_output_enabled (void)
{
    return elog.output_enabled;
}

/**
 * lock output
 */
bool elog_output_lock (bool is_isr)
{
    if (elog.output_lock_enabled)
    {
        elog.output_is_locked_before_disable = true;
        if (is_isr)
        {
            return elog_port_output_lock_isr();
        }
        else
        {
            return elog_port_output_lock();
        }
    }
    else
    {
        elog.output_is_locked_before_enable = true;
        return true;
    }
}

/**
 * unlock output
 */
bool elog_output_unlock (bool is_isr)
{
    if (elog.output_lock_enabled)
    {
        elog.output_is_locked_before_disable = false;
        if (is_isr)
        {
            return elog_port_output_unlock_isr();
        }
        else
        {
            return elog_port_output_unlock();
        }
    }
    else
    {
        elog.output_is_locked_before_enable = false;
        return true;
    }
}

/**
 * output the log
 *
 * @param level level
 * @param tag tag
 * @param file file name
 * @param func function name
 * @param line line number
 * @param format output format
 * @param ... args
 *
 */
void elog_output (bool is_isr, uint8_t level, const char *tag, const char *file, const char *func, const long line,
                  const char *format, ...)
{
    extern elog_timestamp_t elog_port_get_time(void);

    /* check output enabled */
    if (!elog.output_enabled)
    {
        return;
    }

    // Create header for current log
    elog_header_t log_header = {0};

    /* args point to the first variable parameter */
    va_list args;
    va_start(args, format);
    bool is_success = elog_output_lock(is_isr);
    if (!is_success)
    {
        // If we fail to get the lock, increase the sequence number to indicate a skipped log message
        g_seq_num++;
        return;
    }

    // Add sequence number to the log line
    log_header.seq_num = g_seq_num++;

    log_header.level = level;

    log_header.timestamp = elog_port_get_time();

    /* skip the space for header and package other log data to buffer.*/
    size_t header_size = sizeof(elog_header_t);
    int    fmt_result  = vsnprintf(line_log_buf + header_size, ELOG_LINE_BUF_SIZE - header_size, format, args);

    va_end(args);

    if (fmt_result < 0)
    {
        // failed to format the log message, so we should not output it
        elog_output_unlock(is_isr);
        return;
    }

    if (fmt_result > (ELOG_LINE_BUF_SIZE - header_size))
    {
        fmt_result = ELOG_LINE_BUF_SIZE - header_size;
    }

    size_t log_len = fmt_result;

    // If last character is not a newline, add one
    size_t newline_len = strlen(ELOG_NEWLINE_SIGN);
    size_t log_end     = header_size + log_len;
    if (strcmp(line_log_buf + log_end - newline_len, ELOG_NEWLINE_SIGN) != 0)
    {
        if ((log_end + newline_len) > ELOG_LINE_BUF_SIZE)
        {
            memcpy(line_log_buf + ELOG_LINE_BUF_SIZE - newline_len, ELOG_NEWLINE_SIGN, newline_len);
            log_len += ELOG_LINE_BUF_SIZE;
        }
        else
        {
            memcpy(line_log_buf + log_end, ELOG_NEWLINE_SIGN, newline_len);
            log_len += newline_len;
        }
    }

    // Copy the log header into the buffer
    log_header.message_length = log_len;
    memcpy(line_log_buf, &log_header, sizeof(elog_header_t));

/* output log */
#if defined(ELOG_ASYNC_OUTPUT_ENABLE)
    extern void elog_async_output(uint8_t level, const char *log, size_t size);
    elog_async_output(level, line_log_buf, log_header.message_length + sizeof(elog_header_t));
#else
    elog_port_output(line_log_buf, log_len);
#endif
    /* unlock output */
    elog_output_unlock(is_isr);
}

/**
 * enable or disable logger output lock
 * @note disable this lock is not recommended except you want output system exception log
 *
 * @param enabled true: enable  false: disable
 */
void elog_output_lock_enabled (bool enabled)
{
    elog.output_lock_enabled = enabled;
    /* it will re-lock or re-unlock before output lock enable */
    if (elog.output_lock_enabled)
    {
        if (!elog.output_is_locked_before_disable && elog.output_is_locked_before_enable)
        {
            /* the output lock is unlocked before disable, and the lock will unlocking after enable */
            elog_port_output_lock();
        }
        else if (elog.output_is_locked_before_disable && !elog.output_is_locked_before_enable)
        {
            /* the output lock is locked before disable, and the lock will locking after enable */
            elog_port_output_unlock();
        }
    }
}
