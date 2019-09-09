/*
 * block_data.c
 *
 *  Created on: 2019-6-22
 *      Author: Brant
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compress.h"
#include "block_data.h"


#define ALIGN_MEM_WITH_UNIT(n, unit)	((((n)+(unit)-1)/(unit))*(unit))


static int check_file_header(unsigned char *header, const char *mark, int min_ver, int max_ver)
{
	unsigned int version;
	unsigned int block_header_size;

	/* check mark */
	if (strncmp((const char *)header, mark, 4) != 0)
		return -1;

	/* check version */
	memcpy(&version, header + 4, 4);
	if (version < min_ver || version > max_ver)
		return -2;

	/* block header size */
	memcpy(&block_header_size, header + 12, 4);
	if (block_header_size != sizeof(DataBlockHeader))
		return -3;

	return 0;
}

static int read_block_header(BlockDataReader *reader)
{
	unsigned int block_num = 0;
	DataBlockHeader *blocks;
	unsigned int block_header_size;

	reader->read(reader->handle, 8, (unsigned char *)&block_num, 4);
	block_header_size = block_num * sizeof(DataBlockHeader);
	blocks = (DataBlockHeader *)malloc(block_header_size);
	if (blocks == NULL)
		return -1;

	if (reader->read(reader->handle, 16, (unsigned char *)blocks, block_header_size) != block_header_size)
	{
		free(blocks);
		return -2;
	}

	reader->block_num = block_num;
	reader->data_blocks = blocks;

	return 0;
}

static int init_reader(BlockDataReader *reader, const char *mark, int min_ver, int max_ver)
{
	unsigned char file_header[16];

	if (reader->read(reader->handle, 0, file_header, sizeof(file_header)) != sizeof(file_header))
		return -1;

	/* check file header */
	if (check_file_header(file_header, mark, min_ver, max_ver) != 0)
		return -2;

	/* read block header */
	if (read_block_header(reader) != 0)
		return -3;

	return 0;
}


static DataBlockHeader* find_data_block(BlockDataReader *reader, const char *id)
{
	int i;
	for (i = 0; i < reader->block_num; i++)
	{
		if (strncmp(reader->data_blocks[i].id, id, 4) == 0)
		{
			return reader->data_blocks + i;
		}
	}
	return NULL;
}

static unsigned int get_data_block_original_size(DataBlockHeader *block)
{
	return block->original_size != 0 ? block->original_size : block->compress_size;
}

unsigned int data_block_total_size(BlockDataReader *reader, const char *id)
{
	DataBlockHeader *block;

	block = find_data_block(reader, id);
	if (block == NULL)
		return 0;

	return get_data_block_original_size(block);
}

static int read_file(BlockDataReaderHandle handle, unsigned int offset, unsigned char *buf, unsigned int length)
{
	FILE *file = (FILE *)handle;

	fseek(file, offset, SEEK_SET);
	return fread(buf, 1, length, file);
}

static void close_file(BlockDataReaderHandle handle)
{
	fclose((FILE *)handle);
}

BlockDataReader* block_data_reader_with_file(const char *filename, const char *mark, int min_ver, int max_ver)
{
	FILE *data_file;
	BlockDataReader *reader;

	data_file = fopen(filename, "rb");
	if (data_file == NULL)
		return NULL;

	reader = (BlockDataReader *)malloc(sizeof(BlockDataReader));
	if (reader == NULL)
		return NULL;

	reader->handle = data_file;
	reader->read = read_file;
	reader->close_handle = close_file;

	if (init_reader(reader, mark, min_ver, max_ver) != 0)
	{
		free(reader);
		fclose(data_file);
		return NULL;
	}

	return reader;
}


int block_data_read(BlockDataReader *reader, const char *id, unsigned int offset, unsigned char *buf, unsigned int length)
{
	DataBlockHeader *block;
	unsigned char *compress_data = NULL, *original_data = NULL;
	const int more_space = 24;
	unsigned int original_size = 0;

	block = find_data_block(reader, id);
	if (block == NULL)
		return -1;

	if (offset + length > get_data_block_original_size(block))
		return -2;

	/* if data is uncompressed, just read it */
	if (block->original_size == 0)
	{
		return reader->read(reader->handle, block->offset + offset, buf, length);
	}

	/* read and uncompress data */
	compress_data = (unsigned char *)malloc(ALIGN_MEM_WITH_UNIT(block->compress_size, 4));
	if (compress_data == NULL)
		return -3;

	original_data = (unsigned char *)malloc(ALIGN_MEM_WITH_UNIT(block->original_size, 4) + more_space);
	if (original_data == NULL)
	{
		free(compress_data);
		return -3;
	}

	if (reader->read(reader->handle, block->offset, compress_data, block->compress_size) != block->compress_size)
	{
		free(compress_data);
		return -4;
	}

	if (uncompress_data(original_data, &original_size, compress_data, block->compress_size) != 0)
	{
		free(compress_data);
		free(original_data);
		return -5;
	}

	memcpy(buf, original_data + offset, length);
	free(compress_data);
	free(original_data);

	return length;
}


void delete_block_data_reader(BlockDataReader *reader)
{
	if (!reader)
		return;

	if (reader->close_handle)
		reader->close_handle(reader->handle);

	if (reader->data_blocks)
		free(reader->data_blocks);

	free(reader);
}
