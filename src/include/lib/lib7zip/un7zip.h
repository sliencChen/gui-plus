/*
 * un7zip.h
 *
 *  Created on: 2019-6-23
 *      Author: Brant
 */

#ifndef UN7ZIP_H_
#define UN7ZIP_H_


#define LZMA_BASE_SIZE  1846
#define LZMA_LIT_SIZE   768
#define LZMA_BUFFER_SIZE     ((LZMA_BASE_SIZE + (LZMA_LIT_SIZE << (0 + 2))) * sizeof(unsigned short))


int un7zip(unsigned char *in, unsigned char *out, unsigned char *buf);


#endif /* UN7ZIP_H_ */
