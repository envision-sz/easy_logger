/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
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
 * Function: Portable interface for each platform.
 * Created on: 2015-04-28
 */

#include <elog.h>
#include <drivers/ps/rtc.h>
#include <xil_printf.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

#define TIMESTAMO_STR_LENGTH       100
#define GET_LOG_MUTEX_TIMEOUT_MS   1000
#define FORMATTED_LOG_LINE_MAX_LEN 1024

static SemaphoreHandle_t g_log_output_lock = NULL;
static StaticSemaphore_t g_log_output_lock_buf;
static char              g_formatted_log[FORMATTED_LOG_LINE_MAX_LEN]; // For UART output

static size_t format_log (const char *log_input, char *output, size_t size);

/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init (void)
{
    ElogErrCode result = ELOG_NO_ERR;

    /* add your code here */

    return result;
}

/**
 * EasyLogger port deinitialize
 *
 */
void elog_port_deinit (void)
{

    /* add your code here */
}

/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
void elog_port_output (const char *log, size_t size)
{

    /* add your code here */
}

/**
 * output lock in interrupt context
 */
bool elog_port_output_lock_isr (void)
{

    /* add your code here */
}

/**
 * output unlock in interrupt context
 */
bool elog_port_output_unlock_isr (void)
{
    /* add your code here */
}

/**
 * output lock
 */
bool elog_port_output_lock (void)
{
    /* add your code here */
}

/**
 * output unlock
 */
bool elog_port_output_unlock (void)
{
    /* add your code here */
}

/**
 * get current time interface
 *
 * @return current time
 */
const elog_timestamp_t elog_port_get_time (void)
{
    /* add your code here */
}
