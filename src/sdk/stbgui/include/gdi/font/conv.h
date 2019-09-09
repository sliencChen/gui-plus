/*
 * conv.h
 *
 *  Created on: 2016-7-22
 *      Author: Brant
 */

#ifndef _CONV_H_
#define _CONV_H_

/* 0x7f is a control character, use it as unsupported char */
#define WINDOWS_UNSUPPORT_CHAR		0x7F
#define UTF8_UNSUPPORT_CHAR			0x7F
#define UCS2_UNSUPPORT_CHAR			0xFFFF

#define WINDOWS_ENCODING_START		0x10


typedef enum _iso_encoding_
{
	ISO_ENCODING_8859_1 = 1,
	ISO_ENCODING_8859_2,
	ISO_ENCODING_8859_3,
	ISO_ENCODING_8859_4,
	ISO_ENCODING_8859_5,
	ISO_ENCODING_8859_6,
	ISO_ENCODING_8859_7,
	ISO_ENCODING_8859_8,
	ISO_ENCODING_8859_9,
	ISO_ENCODING_8859_10,
	ISO_ENCODING_8859_11,
	ISO_ENCODING_8859_12,
	ISO_ENCODING_8859_13,
	ISO_ENCODING_8859_14,
	ISO_ENCODING_8859_15,
	ISO_ENCODING_NUM
} IsoEncoding;

typedef enum _windows_encoding_
{
	WINDOWS_ENCODING_GB2312 = WINDOWS_ENCODING_START,
	WINDOWS_ENCODING_874,
	WINDOWS_ENCODING_1250,
	WINDOWS_ENCODING_1251,
	WINDOWS_ENCODING_1252,
	WINDOWS_ENCODING_1253,
	WINDOWS_ENCODING_1254,
	WINDOWS_ENCODING_1255,
	WINDOWS_ENCODING_1256,
	WINDOWS_ENCODING_1257,
	WINDOWS_ENCODING_1258,
	WINDOWS_ENCODING_JIS,
	WINDOWS_ENCODING_BIG5,
	WINDOWS_ENCODING_KSC5601,
	WINDOWS_ENCODING_NUM
} WindowsEncoding;

typedef enum _multi_lan_encoding_
{
	/*
	 * ISO-6937 is not a complete multi language encoding, it's only for languages which use Latin alphabet
	 * in some non-standard stream, we need use iso6937 with iso8859-1/iso8859-2... combined encoding.
	 * real encoding = MULTI_LAN_ENCODING_ISO6937 | ISO_ENCODING_8859_1/ISO_ENCODING_8859_2 ...
	 */
	MULTI_LAN_ENCODING_ISO6937		= 0x00000100,
	MULTI_LAN_ENCODING_UNICODE		= 0x0000FFFE,
	MULTI_LAN_ENCODING_UNICODE_BE	= 0x0000FEFF,
	MULTI_LAN_ENCODING_UTF8			= 0x00EFBBBF,
} MultiLanEncoding;


typedef enum
{
	CONV_ERR_CODE_NONE,
	CONV_ERR_CODE_BAD_PARAMETER,
	CONV_ERR_CODE_NO_ENOUGH_SPACE,
	CONV_ERR_CODE_UNSUPPORTED_CHAR,
	CONV_ERR_CODE_INVALID_ENCODING,
	CONV_ERR_CODE_OTHERS,
	CONV_ERR_CODE_NUM
} ConvErrCode;


BOOL is_iso_encoding(U32 encoding);
U8 compatible_windows_encoding(IsoEncoding iso_encoding);

int iso_to_ucs2(const char *iso_string, int iso_string_length, U16 *ucs2_string, int ucs2_size, IsoEncoding encoding);
int iso_to_utf8(const char *iso_string, int iso_string_length, char *utf8_string, int utf8_size, IsoEncoding encoding);

BOOL is_iso6937_encoding(U32 encoding);
int iso6937_to_ucs2(const char *iso6937_string, int string_length, U16 *ucs2_string, int ucs2_size, U32 encoding);
int iso6937_to_utf8(const char *iso6937_string, int string_length, char *utf8_string, int utf8_size, U32 encoding);

BOOL is_windows_encoding(U32 encoding);
BOOL is_windows_string(const char *string);
U16 windows_char_to_ucs2_char(U16 windows_char, WindowsEncoding encoding);

int ucs2_to_windows(const U16 *ucs2_string, int ucs2_length, char *win_string, int win_string_size);
int windows_to_ucs2(const char *win_string, int win_string_length, U16 *ucs2_string, int ucs2_size, WindowsEncoding default_encoding);

int utf8_to_windows(const char *utf8_string, int utf8_length, char *win_string, int win_string_size);
int windows_to_utf8(const char *win_string, int win_string_length, char *utf8_string, int utf8_size, WindowsEncoding default_encoding);

int ucs2_be_to_ucs2(U16 *ucs2_be_string, int ucs2_be_length, U16 *ucs2_string, int ucs2_size);
int ucs2_be_to_utf8(const U16 *ucs2_be_string, int ucs2_be_length, char *utf8_string, int utf8_size);

int ucs2_char_to_utf8_buf(U16 ucs2_char, char *utf8_buf);
int ucs2_to_utf8_bytes_num(const U16 *ucs2_string, int ucs2_length);
int convert_to_utf8_bytes_num(const char *data, int data_length, int encoding);

int ucs2_to_utf8(const U16 *ucs2_string, int ucs2_length, char *utf8_string, int utf8_size);
int utf8_to_ucs2(const char *utf8_string, int utf8_length, U16 *ucs2_string, int ucs2_size);

int utf8_copy(const char *src, int src_length, char *dst, int dst_size);
int convert_to_utf8(const char *data, int data_length, char *utf8_string, int utf8_size, int encoding);

int convert_to_ucs2(const char *data, int data_length, U16 *ucs2_string, int ucs2_size, int encoding);

int ucs2_to_lower(U16 *ucs2_string, int ucs2_length);
int ucs2_to_upper(U16 *ucs2_string, int ucs2_length);

int german_latin_char_to_ascii(U16 *ger_string, int length);

char *get_encoding_name(U32 encoding);

#endif /* _CONV_H_ */
