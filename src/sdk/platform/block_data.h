/*
 * block_data.h
 *
 *  Created on: 2019-6-22
 *      Author: Brant
 */

#ifndef BLOCK_DATA_H_
#define BLOCK_DATA_H_

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct _data_block_header_
{
	char id[4];						///< id of data block
	unsigned int  offset;			///< offset of data block in this file
	unsigned int  compress_size;	///< length of data block
	unsigned int  original_size;	///< length before compress, set to 0 if data haven't been compressed
	unsigned int  checksum;			///< checksum of data block
} DataBlockHeader;


typedef void* BlockDataReaderHandle;
typedef struct _block_data_reader_
{
	BlockDataReaderHandle handle;
	unsigned int block_num;
	DataBlockHeader *data_blocks;

	int  (*read)(BlockDataReaderHandle handle, unsigned int offset, unsigned char *buf, unsigned int length);
	void (*close_handle)(BlockDataReaderHandle handle);
} BlockDataReader;


BlockDataReader* block_data_reader_with_file(const char *filename, const char *mark, int min_ver, int max_ver);
void delete_block_data_reader(BlockDataReader *reader);

unsigned int data_block_total_size(BlockDataReader *reader, const char *id);
int block_data_read(BlockDataReader *reader, const char *id, unsigned int offset, unsigned char *buf, unsigned int length);


#ifdef __cplusplus
}
#endif

#endif /* BLOCK_DATA_H_ */
