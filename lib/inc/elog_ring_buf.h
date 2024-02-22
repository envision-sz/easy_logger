#ifndef _ELOG_BUF_H
#define _ELOG_BUF_H

#include <stdint.h>
#include <stddef.h>

size_t elog_buf_used(void);

size_t elog_buf_avail(void);

int elog_buf_push(const char *log, uint16_t size);

int elog_buf_pop(char *log, uint16_t size);

// Peek the top log in the buffer
int elog_buf_peek(elog_header_t *header);
#endif // _ELOG_BUF_H