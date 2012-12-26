/*
 * lib/libc/fifo.c
 */

#include <types.h>
#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include <fifo.h>

#ifndef MIN
#define MIN(a, b)	((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)	((a) > (b) ? (a) : (b))
#endif

/*
 * allocates a new fifo using a preallocated buffer
 *
 * buffer: the preallocated buffer to be used.
 * size: the size of the internal buffer.
 */
struct fifo * fifo_init(u8_t * buffer, size_t size)
{
	struct fifo * fifo;

	fifo = malloc(sizeof(struct fifo));
	if(!fifo)
		return NULL;

	fifo->buffer = buffer;
	fifo->size = size;
	fifo->in = fifo->out = 0;

	return fifo;
}

/*
 * allocates a new fifo and its internal buffer
 *
 * size: the size of the internal buffer to be allocated.
 */
struct fifo * fifo_alloc(size_t size)
{
	u8_t * buffer;
	struct fifo * fifo;

	buffer = malloc(size);
	if(!buffer)
		return NULL;

	fifo = fifo_init(buffer, size);
	if(!fifo)
		free(buffer);

	return fifo;
}

/*
 * frees the fifo
 */
void fifo_free(struct fifo * fifo)
{
	free(fifo->buffer);
	free(fifo);
}

/*
 * removes the entire fifo contents
 */
void fifo_reset(struct fifo * fifo)
{
	fifo->in = fifo->out = 0;
}

/*
 * fifo_len - returns the number of bytes available in the FIFO
 *
 * fifo: the fifo to be used.
 */
size_t fifo_len(struct fifo * fifo)
{
	return fifo->in - fifo->out;
}

/*
 * fifo_put - puts some data into the fifo
 *
 * fifo: the fifo to be used.
 * buffer: the data to be added.
 * len: the length of the data to be added.
 *
 * This function copies at most len bytes from the buffer into
 * the FIFO depending on the free space, and returns the number of
 * bytes copied.
 */
size_t fifo_put(struct fifo * fifo, u8_t * buffer, size_t len)
{
	size_t l;

	len = MIN(len, fifo->size - fifo->in + fifo->out);
	if(len == 0)
		return 0;

	/* first put the data starting from fifo->in to buffer end */
	l = MIN(len, fifo->size - (fifo->in % fifo->size));
	memcpy(fifo->buffer + (fifo->in % fifo->size), buffer, l);

	/* then put the rest (if any) at the beginning of the buffer */
	memcpy(fifo->buffer, buffer + l, len - l);

	fifo->in += len;

	return len;
}

/*
 * fifo_get - gets some data from the fifo
 *
 * fifo: the fifo to be used.
 * buffer: where the data must be copied.
 * len: the size of the destination buffer.
 *
 * This function copies at most len bytes from the fifo into the
 * buffer and returns the number of copied bytes.
 */
size_t fifo_get(struct fifo * fifo, u8_t * buffer, size_t len)
{
	u32_t l;

	len = MIN(len, fifo->in - fifo->out);
	if(len == 0)
		return 0;

	/* first get the data from fifo->out until the end of the buffer */
	l = MIN(len, fifo->size - (fifo->out % fifo->size));
	memcpy(buffer, fifo->buffer + (fifo->out % fifo->size), l);

	/* then get the rest (if any) from the beginning of the buffer */
	memcpy(buffer + l, fifo->buffer, len - l);

	fifo->out += len;

	if (fifo->in == fifo->out)
		fifo->in = fifo->out = 0;

	return len;
}
