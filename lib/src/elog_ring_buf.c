/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2016, Armink, <armink.ztl@gmail.com>
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
 * Function: Logs buffered output.
 * Created on: 2016-11-09
 */

#include <elog.h>
#include <string.h>

/* buffer size for asynchronous output mode */
#ifdef ELOG_ASYNC_OUTPUT_BUF_SIZE
    #define RING_BUF_SIZE ELOG_ASYNC_OUTPUT_BUF_SIZE
#else
    #define RING_BUF_SIZE (ELOG_LINE_BUF_SIZE * 10)
#endif /* ELOG_ASYNC_OUTPUT_BUF_SIZE */

/* asynchronous output mode's ring buffer */
static char ring_buf[RING_BUF_SIZE] = {0};

// current size of data in the buffer
static size_t data_size = 0;
/* log ring buffer write index */
static size_t write_index = 0;
/* log ring buffer read index */
static size_t read_index = 0;

size_t elog_buf_used (void)
{
    return data_size;
}

size_t elog_buf_avail (void)
{
    return RING_BUF_SIZE - data_size;
}

int elog_buf_push (const char *log, size_t size)
{
    size_t available = elog_buf_avail();
    if (size > available){
    	return -1;
    }

	size_t copy_size = size;
	if (write_index + copy_size > RING_BUF_SIZE)
	{
		// wrap around
		size_t first_chunk = RING_BUF_SIZE - write_index;
		memcpy(&ring_buf[write_index], log, first_chunk);
		memcpy(&ring_buf[0], &log[first_chunk], copy_size - first_chunk);
		write_index = copy_size - first_chunk;
	}
	else
	{
		memcpy(&ring_buf[write_index], log, copy_size);
		write_index += copy_size;
	}
	data_size += copy_size;
	return 0;

}

int elog_buf_pop (char *log, uint16_t size)
{
    if (size > data_size)
    {
        // can't pop it
        return -1;
    }

    if (read_index + size > RING_BUF_SIZE)
    {
        // wrap around
        size_t first_chunk = RING_BUF_SIZE - read_index;
        memcpy(log, &ring_buf[read_index], first_chunk);
        memcpy(&log[first_chunk], &ring_buf[0], size - first_chunk);
        read_index = size-first_chunk;
    }
    else
    {
        memcpy(log, &ring_buf[read_index], size);
        read_index += size;
    }
    data_size -= size;
    return 0;
}

int elog_buf_peek (elog_header_t *header)
{
    if (data_size < sizeof(elog_header_t))
    {
        // can't peek it
        return -1;
    }
    if (read_index + sizeof(elog_header_t) > RING_BUF_SIZE)
    {
        // wrap around
        memcpy(header, &ring_buf[read_index], RING_BUF_SIZE - read_index);
        memcpy((char *)header + RING_BUF_SIZE - read_index,
               &ring_buf[0],
               sizeof(elog_header_t) - (RING_BUF_SIZE - read_index));
    }
    else
    {
        memcpy(header, &ring_buf[read_index], sizeof(elog_header_t));
    }
    return 0;
}
