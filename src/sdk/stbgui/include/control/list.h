/*
 * list.h
 *
 *  Created on: 2018-8-6
 *      Author: Brant
 */

#ifndef _GUI_LIST_H_
#define _GUI_LIST_H_

#ifdef __cplusplus
extern "C"
{
#endif


typedef UIWindow UIList;
#define LIST_CLASS_NAME "list"

#define LIST_DEFAULT_WIDTH	900
#define LIST_DEFAULT_HEIGHT	500

#define HOR_DIRECTION   (0)
#define VER_DIRECTION   (1)

#define MAX_COLUMN_NUM		    13
#define MAX_BYTES_PER_COLUMN	60

typedef struct _ui_list_column_
{
	U32 tag;
	BOOL visiable;
	int min_width;
	int proportion;
}UIListColumn;

typedef struct _ui_list_bar_data_
{
	BOOL enable_bar;
	const char *up_bmp;
	const char *scroll_bar_bmp;
	const char *down_bmp;
	U32   bk_color;
	U32   scroll_bar_color;
	U32   triangle_color;
	int bar_offset;
	int bar_width;
}UIListBarData;

typedef struct _ui_list_data_
{
	/* property */
	int select;
	int prev_select;
	int total;

	int iColStart;
	int iRowStart;

	int page_max_item;
	int cur_page;
	int total_page;

	int item_height;
	int item_space;

	/* list bar */
	UIListBarData bar_data;

	/* text */
	int offset;

	BOOL  direction;//true -ver_direction ,false -hor_direction
	struct _ui_list_column_ column_data[MAX_COLUMN_NUM];

	/* internal use */
	BOOL (*read_column_data_function)(struct _ui_list_data_ *list_data, int col_tag, int row_index, char *buffer, BOOL focus);
} UIListData;

UIList* create_list(const char *id);

/* list theme */
typedef struct _ui_list_theme_common_data_
{
	int   font_size;
	U32   font_color;
	U32   light_font_color;
}UIListThemeCommonData;

typedef struct _ui_list_bmprect_theme_
{
	UIListThemeCommonData common_data;
	const char *focus_bmp;
	const char *unfocus_bmp;
	U32   bk_color;
	U32   light_bk_color;
	U32   bk_color1;
	U32   light_bk_color1;
}UIListThemeBmpWithRect;

BOOL draw_list_theme_bmp_with_rect(UIList *list, UIListThemeBmpWithRect *theme_param, int index, BOOL focus);

typedef struct _ui_list_bmp_theme_
{
	UIListThemeCommonData common_data;
	const char *focus_bmp;
	const char *unfocus_bmp;
}UIListThemeBmp;

BOOL draw_list_theme_bmp(UIList *list, UIListThemeBmp *theme_param, int index, BOOL focus);

typedef struct _ui_list_rect_theme_
{
	UIListThemeCommonData common_data;
	U32   bk_color;
	U32   light_bk_color;
	U32   bk_color1;
	U32   light_bk_color1;
}UIListThemeRect;

BOOL draw_list_theme_rect(UIList *list, UIListThemeRect *theme_param, int index, BOOL focus);
BOOL read_tv_favorites_item(struct _ui_list_data_ *list_data, int col_tag, int row_index, char *buffer, BOOL focus);
/* list function */
BOOL set_list_total_width(UIList *list, int width);
BOOL set_list_item_height(UIList *list, int height);
BOOL set_list_item_space(UIList *list, int space);
BOOL set_list_total_num(UIList *list, int total);
BOOL set_list_offset(UIList *list, int offset);
BOOL set_list_page_max_item(UIList *list, int max_item);
BOOL set_list_position(UIList *list, int col, int row);
BOOL set_list_bar_color(UIList *list, UIColor back_color, UIColor scroll_bar_color);
BOOL set_list_bar_scroll_bar_bmp(UIList *list, const char *bmp_name);
BOOL set_list_bar_offset(UIList *list, int bar_offset);
BOOL enable_list_bar(UIList *list, BOOL enable);
BOOL read_item_example(struct _ui_list_data_ *list_data, int col_tag, int row_index, char *buffer, BOOL focus);
BOOL set_list_read_column_data_function(UIList *list, BOOL (*read_column_data_function)(struct _ui_list_data_ *list_data, int col_tag, int row_index, char *buffer, BOOL focus));

#ifdef __cplusplus
}
#endif

#endif /* _GUI_LIST_H_ */
