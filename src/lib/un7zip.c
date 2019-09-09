/*
 * un7zip.c
 *
 *  Created on: 2017-7-21
 *      Author: Brant
 */

#include <string.h>
#include "lzma/LzmaDec.h"
#include "lzma/LzmaLib.h"


int _7zip(unsigned char *in, unsigned int insize, unsigned char *out, unsigned int *outsize)
{
	__int64 SrcLen = insize;
	size_t DstLen;
	unsigned char *DstBuf;
	size_t outPropsSize = 5;
	unsigned char header[13];
	int i;

	DstLen = (size_t)SrcLen * 2;
	DstBuf = malloc(DstLen);
	if (DstBuf == NULL)
	{
		return -1;
	}

	LzmaCompress(DstBuf, &DstLen, in, (unsigned int)SrcLen, header, &outPropsSize, 5, (1 << 23), 0, 2, 2, 128, 2);
	for (i = 0; i < 8; i++)
	{
		header[outPropsSize++] = (unsigned char)(SrcLen >> (8 * i));
	}
	if (*outsize < DstLen + 13)
	{
		free(DstBuf);
		return -1;
	}

	memcpy(out, header, 13);
	memcpy(out + 13, DstBuf, DstLen);
	*outsize = DstLen + 13;

	free(DstBuf);

	return 0;
}

int un7zip(unsigned char *in, unsigned char *out, unsigned char *buf)
{
    unsigned char prop[5];
    unsigned int prop_len = 5;
    unsigned int outSize;
    unsigned int inSize = 0x2000000;

    outSize = ((in[8] << 24) | (in[7] << 16) | (in[6] << 8) | (in[5]));
	memcpy(prop, in, 5);
	return LzmaUncompress(out, &outSize, in + 13, &inSize, prop, prop_len);
}
