/*
 * compress.h
 *
 *  Created on: 2019-6-24
 *      Author: Brant
 */

#ifndef GUI_COMPRESS_H_
#define GUI_COMPRESS_H_


int compress_data(unsigned char *buf_in, unsigned int in_size, unsigned char *buf_out, unsigned int *out_size);
int uncompress_data(unsigned char *buf_out, unsigned int *out_size, unsigned char *buf_in, unsigned int in_size);


#endif /* GUI_COMPRESS_H_ */
