/*
 * gui_typedef.h
 *
 *  Created on: 2019-3-17
 *      Author: Brant
 */

#ifndef _GUI_TYPEDEF_H_
#define _GUI_TYPEDEF_H_


//-------------------------------------------------------------------------------------------------
// basic
//-------------------------------------------------------------------------------------------------
typedef unsigned char  U8;
typedef signed short   S16;
typedef unsigned short U16;
typedef signed int     S32;
typedef unsigned int   U32;

typedef int            BOOL;

#ifndef TRUE
#define TRUE           1
#endif

#ifndef FALSE
#define FALSE          0
#endif


//-------------------------------------------------------------------------------------------------
// rect
//-------------------------------------------------------------------------------------------------
typedef struct _ui_point_
{
	int x;
	int y;
} UIPoint;

typedef struct _ui_size_
{
	int width;
	int height;
} UISize;

typedef struct _ui_rect_
{
	UIPoint point;
	UISize size;
} UIRect;

typedef struct _ui_inset_
{
	int left;
	int top;
	int right;
	int bottom;
} UIInset;


//-------------------------------------------------------------------------------------------------
// color
//-------------------------------------------------------------------------------------------------
typedef unsigned int	UIColor;


//-------------------------------------------------------------------------------------------------
// canvas
//-------------------------------------------------------------------------------------------------
typedef struct ui_pixel_buf_t
{
	void*		addr;
	UISize 		size;
	U8			color_mode;
	U8			pixel_size;

	U32  (*conv_color)(struct ui_pixel_buf_t *self, UIColor color);						///< convert a ARGB8888 color to canvas format

	U32  (*get_pixel)(struct ui_pixel_buf_t *self, int x, int y);						///< pixel data use canvas color format
	void (*set_pixel)(struct ui_pixel_buf_t *self, int x, int y, U32 color);			///< pixel data use canvas color format

//	void (*set_pixel_alpha)(struct ui_pixel_buf_t *self, int x, int y, U8 alpha);		///< set alpha for pixel

	void* (*get_buf)(struct ui_pixel_buf_t *self, int x, int y);						///< get the pointer of pixels start with x,y
	void  (*set_buf)(struct ui_pixel_buf_t *self, int x, int y, void *pixels, int pixel_num);

	void (*blend_8888)(struct ui_pixel_buf_t *self, int x, int y, U32 color);			///< blend a pixel with ARGB8888 format to current canvas
	void (*blend_1555)(struct ui_pixel_buf_t *self, int x, int y, U32 color);			///< blend a pixel with ARGB1555 format to current canvas

	void (*mix_8888)(struct ui_pixel_buf_t *self, int x, int y, U32 color);				///< mix a pixel with ARGB8888 format to current canvas
//	void (*mix_1555)(struct ui_pixel_buf_t *self, int x, int y, U32 color);				///< mix a pixel with ARGB1555 format to current canvas
} UIPixelBuf;

typedef UIPixelBuf	UICanvas;


//-------------------------------------------------------------------------------------------------
// bmp
//-------------------------------------------------------------------------------------------------
typedef struct _ui_bmp_
{
	UIPixelBuf pixel_buf;
	U32 has_trans: 1;
	U32 reserved: 31;
} UIBmp;

enum
{
	BMP_SLICE_HOR	= 0x01,
	BMP_SLICE_VER	= 0x02,
	BMP_SLICE_BOTH  = BMP_SLICE_HOR|BMP_SLICE_VER
};

enum
{
	BMP_SLICE_FILL_TILE		= 0x01,
	BMP_SLICE_FILL_STRETCH	= 0x02
};

typedef struct _bmp_slice_
{
	U32 slice_mode	:2;
	U32 left		:15;
	U32 top			:15;
	U32 fill_mode	:2;
	U32 fill_width	:15;
	U32 fill_height	:15;
} BmpSlice;


#endif /* _GUI_TYPEDEF_H_ */
