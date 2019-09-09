#ifndef _FONT_METRIX_H_
#define _FONT_METRIX_H_

#include "font_max.h"


/* X_TECH true-type struct */
typedef signed char GS_TT_POS;
typedef short GS_TT_POS_BIG;

typedef struct _gs_vector_
{
	GS_TT_POS x;
	GS_TT_POS y;
} GS_TT_VECTOR;

typedef struct _gs_vector_big_
{
	GS_TT_POS_BIG x;
	GS_TT_POS_BIG y;
} GS_TT_VECTOR_BIG;

typedef struct _gs_outline_
{
	U16 ucContoursNum;			/* number of contours in glyph */
	U16 ucPointsNum;			/* number of points in the glyph */
	GS_TT_VECTOR_BIG astPoints[FONT_MAX_POINT];/* the outline's points */
	U8 *acTags;					/* the points flags */
	U8 *acContours;				/* the contour end points */
} GS_TT_OUTLINE;

typedef struct gs_bitmap_
{
	int iLeft;
	int iTop;
	int iWidth;
	int iHeight;
	U8 *pucBuffer;
} GS_TT_BITMAP;


/* metrics */
typedef struct _outline_metrics_
{
	U8 contours_num;	/* number of contours in glyph */
	U8 points_num;		/* number of points in the glyph */
	U16 points_addr;	/* the outline's points */
	U16 tags_addr;		/* the points flags */
	U16 contours_addr;	/* the contour end points */
} GS_TT_OUTLINE_METRICS;

typedef struct _outline_metrics_big_
{
	U16 contours_num;	/* number of contours in glyph */
	U16 points_num;		/* number of points in the glyph */
	U16 points_addr;	/* the outline's points */
	U16 tags_addr;		/* the points flags */
	U16 contours_addr;	/* the contour end points */
} GS_TT_OUTLINE_METRICS_BIG;


typedef struct _char_metrics_
{
	GS_TT_OUTLINE_METRICS	metrics;
	unsigned char			advance;		/* width */
} CHAR_METRICS;

typedef struct _char_metrics_big_
{
	GS_TT_OUTLINE_METRICS_BIG	metrics;
	unsigned char				advance;	/* width */
} CHAR_METRICS_BIG;

typedef struct _metrics_block_
{
	U16 start_index;
	U32 points_base;
	U32 tags_base;
	U32 contours_base;
} METRICS_BLOCK;

typedef struct _char_advance_
{
	U16 char_index;
	U8 advance;
} CHAR_ADVANCE;



/* for char map */
typedef BOOL (*GetMetricsFunc)(U16 index, GS_TT_OUTLINE *outline, int *ascender, int *advance);
typedef U16 (*GetGlypFunc)(U16 index);
typedef U16 (*GetGlypPropsFunc)(U16 glyph_char);
typedef U16 (*GetUnicodePropsFunc)(U16 index);

typedef struct _font_char_func_map_
{
	unsigned short start_char_code;
	unsigned short end_char_code;
	GetMetricsFunc get_metrics_func;
	GetGlypFunc get_glyph_func;
	GetGlypPropsFunc get_glyph_props_func;
	GetUnicodePropsFunc get_unicode_props_func;
} FONT_CHAR_FUNC_MAP;

typedef struct _font_char_offset_map_
{
	unsigned short start_char_code;
	unsigned short end_char_code;
	unsigned short offset;
} FONT_CHAR_OFFSET_MAP;

typedef struct _font_glyph_
{
	GetMetricsFunc font_glyph_get_metrics_func;
	GetGlypPropsFunc font_get_glyph_props_func;
	U16 glyph;
	U16 cluster;
	U16 glyph_props;
	U16 unicode_props;
	U16 mask;
	U16 reserver;
	U8 cat;
	U8 pos;
	U8 syllable;
	U8 lig_props;
}FONT_GLYPH;

typedef struct _font_glyph_map_
{
	FONT_GLYPH *font_fun_glyph;
	int font_glyph_num;
}FONT_GLYPH_MAP;

typedef enum _glyph_props_flags_
{
  /* The following three match LookupFlags::Ignore* numbers. */
  GLYPH_PROPS_BASE_GLYPH	= 0x02,
  GLYPH_PROPS_LIGATURE	= 0x04,
  GLYPH_PROPS_MARK		= 0x08,

  /* The following are used internally; not derived from GDEF. */
  GLYPH_PROPS_SUBSTITUTED	= 0x10,
  GLYPH_PROPS_LIGATED	= 0x20,
  GLYPH_PROPS_MULTIPLIED	= 0x40,

  GLYPH_PROPS_PRESERVE     = GLYPH_PROPS_SUBSTITUTED | GLYPH_PROPS_LIGATED | GLYPH_PROPS_MULTIPLIED
}GLYPH_PROPS_FLAGS;

typedef struct _range_record_
{
	unsigned short start_glyph;
	unsigned short end_glyph;
	unsigned short value;
}RANGE_RECORD;

/* render */
int RenderGlyph(GS_TT_OUTLINE *pstOutline, GS_TT_BITMAP *pstBitmap, int x_percent, int y_percent);
int get_glyph_cbox(GS_TT_OUTLINE *pstOutline, GS_TT_BITMAP *pstBitmap, int x_percent, int y_percent);


int get_real_font_height(void);
int get_real_font_size(void);

BOOL get_none_char_metrics(GS_TT_OUTLINE *outline, int *ascender, int *advance);
BOOL get_common_char_metrics(U16 index, GS_TT_OUTLINE *outline, int *ascender, int *advance);
BOOL get_arabic_char_metrics(U16 index, GS_TT_OUTLINE *outline, int *ascender, int *advance);
BOOL get_thai_char_metrics(U16 index, GS_TT_OUTLINE *outline, int *ascender, int *advance);
BOOL get_khmer_char_metrics(U16 glyph_char, GS_TT_OUTLINE *outline, int *ascender, int *advance);
BOOL get_khmer_glyph_char_metrics(U16 index, GS_TT_OUTLINE *outline, int *ascender, int *advance);
U16 khmer_glyph_get_glyph_props(U16 glyph_index);
U16 khmer_glyph_get_unicode_props(U16 index);
#if ENABLE_CHINESE_LANGUAGE
BOOL get_chinese_char_metrics(U16 index, GS_TT_OUTLINE *outline, int *ascender, int *advance);
#endif

U16 get_unicode_glyph(U16 unicode_char);
void get_unicode_font_glyph(U16 unicode_char, FONT_GLYPH *font_glyph);
void get_char_metrics(FONT_GLYPH font_glyph, GS_TT_OUTLINE *outline, int *ascender, int *advance);
U16 get_char_glyph_props(FONT_GLYPH font_glyph, U16 glyph_char);
U16 get_char_unicode_props(U16 unicode_char);
int GetMeterEnglishCharMetrics(U16 cCharCode, GS_TT_OUTLINE *pstOutline, int *piHeight, int *piAscender, int *piAdvance);


U16 get_khmer_glyph_char(U16 index);

#endif	//_FONT_METRIX_H_
