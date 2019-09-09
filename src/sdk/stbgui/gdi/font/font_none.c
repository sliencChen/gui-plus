/*
 * this file was created by font tool, please do NOT modify it manually. @brant
 *
 * font name: Arial, size: 18, size in pixel: 28
 */

#include <gdi/gui_interface.h>
#include <gdi/font/font_metrics.h>


#define NONE_CHAR_ASCENDER  22

static GS_TT_VECTOR none_char_vector[] =
{
	/* ucs2 code:none, advance:18 */
	{12, 0}, {12, 60}, {60, 60}, {60, 0}, {14, 2}, {58, 2}, {58, 58}, {14, 58},
};

static unsigned char none_char_tags[] =
{
	0xff,
};

static unsigned char none_char_contours[] =
{
	0x03, 0x07,
};

static CHAR_METRICS none_char_metrics[] =
{
	{{0x02, 0x08, 0x0000, 0x0000, 0x0000}, 0x12},
};


int get_real_font_height(void)
{
	return 28;
}

int get_real_font_size(void)
{
	return 18;
}

BOOL get_none_char_metrics(GS_TT_OUTLINE *outline, int *ascender, int *advance)
{

	int count = 0;
	if (outline != NULL)
	{
		outline->ucContoursNum = none_char_metrics[0].metrics.contours_num;
		outline->ucPointsNum = none_char_metrics[0].metrics.points_num;
		for (count = 0; count < outline->ucPointsNum; count++)
		{
			outline->astPoints[count].x = none_char_vector[none_char_metrics[0].metrics.points_addr + count].x;
			outline->astPoints[count].y = none_char_vector[none_char_metrics[0].metrics.points_addr + count].y;
		}
		outline->acTags = none_char_tags + none_char_metrics[0].metrics.tags_addr;
		outline->acContours = none_char_contours + none_char_metrics[0].metrics.contours_addr;
	}
	if (ascender != NULL)
	{
		*ascender = NONE_CHAR_ASCENDER;
	}
	if (advance != NULL)
	{
		*advance = none_char_metrics[0].advance;
	}
	return TRUE;
}
