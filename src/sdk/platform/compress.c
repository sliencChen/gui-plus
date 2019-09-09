/*
 * compress.c
 *
 *  Created on: 2019-6-24
 *      Author: Brant
 */

#include <stdlib.h>
#include <lib/lib7zip/un7zip.h>


int compress_data(unsigned char *buf_in, unsigned int in_size, unsigned char *buf_out, unsigned int *out_size)
{
	return _7zip(buf_in, in_size, buf_out, out_size);
}

int uncompress_data(unsigned char *buf_out, unsigned int *out_size, unsigned char *buf_in, unsigned int in_size)
{
	unsigned char *lzma_buffer = NULL;

	lzma_buffer = malloc(LZMA_BUFFER_SIZE);
	if (lzma_buffer == NULL)
		return -1;

	if (un7zip(buf_in, buf_out, lzma_buffer) != 0)
	{
		free(lzma_buffer);
		return -2;
	}

	if (out_size)
		*out_size = *(unsigned int *)lzma_buffer;

	free(lzma_buffer);

	return 0;
}
