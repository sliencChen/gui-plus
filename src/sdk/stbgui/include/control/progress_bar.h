/*
 * progress_bar.h
 *
 *  Created on: 2018-9-29
 *      Author: Grace
 */
#ifndef _PROGRESS_BAR_H_
#define _PROGRESS_BAR_H_

#ifdef __cplusplus
extern "C"
{
#endif


typedef UIWindow					UIProgressBar;
#define PROGRESS_BAR_CLASS_NAME		"ProgressBar"


typedef struct _ui_progress_bar_data_
{
	U32 max_value;
	U32 value;
	U32 loading_value;

	U32 show_percent	:1;
	U32 reserved		:31;
} UIProgressBarData;


UIProgressBar* create_progress_bar(const char *id);

int set_progress_bar_max_value(UIProgressBar *pb, U32 max_value);
int set_progress_bar_value(UIProgressBar *pb, U32 value);
int set_progress_bar_show_percent(UIProgressBar *pb, BOOL show_percent);


//-------------------------------------------------------------------------------------------------
// typical theme
//-------------------------------------------------------------------------------------------------
typedef struct _pb_theme_rect_
{
	UIColor bar_color;
	UIColor progress_color;
	UIColor text_color;
	int height;
	int font_size;
	int bar_text_space;
} UIProgressBarThemeRect;

BOOL progress_bar_theme_rect(UIProgressBar *pb, UIProgressBarThemeRect *theme_param);


typedef struct _pb_theme_bmp_
{
	const char *bar_bmp;
	const char *progress_bmp;
	const char *anchor_bmp;
	UIColor text_color;
	int font_size;
	int bar_text_space;
} UIProgressBarThemeBmp;

BOOL progress_bar_theme_bmp(UIProgressBar *pb, UIProgressBarThemeBmp *theme_param);


#ifdef __cplusplus
}
#endif

#endif
