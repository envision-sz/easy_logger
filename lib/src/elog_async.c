/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2016-2017, Armink, <armink.ztl@gmail.com>
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
 * Function: Logs asynchronous output.
 * Created on: 2016-11-06
 */

#include <elog.h>
#include <string.h>
#include <elog_ring_buf.h>

/* the highest output level for async mode, other level will sync output */
#ifdef ELOG_ASYNC_OUTPUT_LVL
    #define OUTPUT_LVL ELOG_ASYNC_OUTPUT_LVL
#else
    #define OUTPUT_LVL ELOG_LVL_ASSERT
#endif /* ELOG_ASYNC_OUTPUT_LVL */

/* asynchronous output mode enabled flag */
static bool is_enabled = false;

extern void elog_port_output (const char *log, size_t size);
extern void elog_output_lock (bool is_isr);
extern void elog_output_unlock (bool is_isr);

/**
 * put log to asynchronous output ring buffer
 *
 * @param log put log buffer
 * @param size log size
 *
 * @return void
 */
static void async_put_log (const char *log, size_t size)
{
    // If the buffer is full, the log will be dropped
    elog_buf_push(log, size);
}

/**
 * Get line log from asynchronous output ring buffer.
 * It will copy all log when the newline sign isn't find.
 *
 * @param log get line log buffer
 * @param size line log size
 *
 * @return get line log size, the log size is less than ring buffer used size
 */
ElogErrCode elog_async_get_line_log (char *log, size_t size)
{

    elog_header_t top_log_header;
    int           ret = elog_buf_peek(&top_log_header);
    if (ret != 0)
    {
        return ELOG_NO_LOG;
    }

    size_t total_log_line_size = top_log_header.message_length + sizeof(elog_header_t);
    if (size < total_log_line_size)
    {
        // Current buf is not big enough to contain the whole log line
        return ELOG_INPUT_ERR;
    }

    elog_buf_pop(log, total_log_line_size);
    return ELOG_NO_ERR;
}

void elog_async_output (uint8_t level, const char *log, size_t size)
{
    if (is_enabled)
    {
        async_put_log(log, size);
    }
    else
    {
        elog_port_output(log, size);
    }
}

/**
 * enable or disable asynchronous output mode
 * the log will be output directly when mode is disabled
 *
 * @param enabled true: enabled, false: disabled
 */
void elog_async_enabled (bool enabled)
{
    is_enabled = enabled;
}
