/*
 * render.h
 *
 *  Created on: 2018-6-29
 *      Author: Brant
 *
 * THis file is a port of FreeType project
 *
 */

#ifndef _RENDER_H_
#define _RENDER_H_

#ifndef NULL
#define NULL  (void*)0
#endif

#define FT_ABS(a)	((a) < 0 ? -(a) : (a))

#define FT_PIX_FLOOR(x)		((x) & ~63)
#define FT_PIX_ROUND(x)		FT_PIX_FLOOR((x) + 32)
#define FT_PIX_CEIL(x)		FT_PIX_FLOOR((x) + 63)

typedef unsigned int	FT_UInt;
typedef signed int		FT_Int;
typedef signed long		FT_Pos;

typedef long	TCoord;   /* integer scanline/pixel coordinate */
typedef long	TPos;     /* sub-pixel coordinate              */
typedef int		TArea;

enum
{
	ErrRaster_None = 0,
	ErrRaster_Invalid_Mode,
	ErrRaster_Invalid_Outline,
	ErrRaster_Memory_Overflow,
	ErrRaster_Invalid_Argument,
	ErrRaster_Others
};

typedef struct FT_Vector_
{
	FT_Pos x;
	FT_Pos y;
} FT_Vector;

typedef struct FT_Outline_
{
	short		n_contours; /* number of contours in glyph        */
	short		n_points; 	/* number of points in the glyph      */

	FT_Vector*	points; 	/* the outline's points               */
	char*		tags; 		/* the points flags                   */
	short*		contours; 	/* the contour end points             */

	int			flags; 		/* outline masks                      */
} FT_Outline;

typedef struct FT_Bitmap_
{
	int 			rows;
	int 			width;
	int 			pitch;
	unsigned char* 	buffer;
	short 			num_grays;
	char 			pixel_mode;
	char 			palette_mode;
	void* 			palette;
} FT_Bitmap;


typedef struct FT_BBox_
{
	FT_Pos xMin, yMin;
	FT_Pos xMax, yMax;
} FT_BBox;

typedef struct TCell_* PCell;

typedef struct TCell_
{
	TPos x; /* same with TWorker.ex */
	TCoord cover; /* same with TWorker.cover */
	TArea area;
	PCell next;
} TCell;

typedef struct FT_Span_
{
	short x;
	unsigned short len;
	unsigned char coverage;
} FT_Span;

typedef struct TBand_
{
	TPos min, max;
} TBand;


#define FT_MAX_GRAY_SPANS  32
typedef void (*FT_SpanFunc)(int y,int count, const FT_Span* spans, void* user);

typedef struct TWorker_
{
	TCoord ex, ey;
	TPos min_ex, max_ex;
	TPos min_ey, max_ey;
	TPos count_ex, count_ey;

	TArea area;
	TCoord cover;
	int invalid;

	PCell cells;
	int max_cells;
	int num_cells;

	TCoord cx, cy;
	TPos x, y;

	TPos last_ey;

	FT_Vector bez_stack[32 * 3 + 1];
	int lev_stack[32];

	FT_Outline outline;
	FT_Bitmap target;
	FT_BBox clip_box;

	FT_Span gray_spans[FT_MAX_GRAY_SPANS];
	int num_gray_spans;

	FT_SpanFunc render_span;
	void* render_span_data;
	int span_y;

	int  band_size;
	int  band_shoot;

//	ft_jmp_buf jump_buffer;

	void* buffer;
	long buffer_size;

	PCell* ycells;
	TPos ycount;
} TWorker, *PWorker;

#endif	//_RENDER_H_
