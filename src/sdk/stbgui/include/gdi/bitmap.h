/*
 * bitmap.h
 *
 *  Created on: 2018-6-26
 *      Author: Brant
 */

#ifndef _GUI_BITMAP_H_
#define _GUI_BITMAP_H_


#define KEY_COLOR_NONE		0
#define INVALID_COPY_POS	0xffff


typedef enum
{
	DRAW_BMP_MODE_NORMAL,
	DRAW_BMP_MODE_VAR_SIZE,
	DRAW_BMP_MODE_STRETCH,
	DRAW_BMP_MODE_REPEAT_RIGHT,
	DRAW_BMP_MODE_PART,
	DRAW_BMP_MODE_ROTATE,
	DRAW_BMP_MODE_NUM
} DrawBmpMode;


typedef struct _rotate_bmp_param_
{
	int angle;
	float w_ratio;
	float h_ratio;
} RotateBmpParam;

typedef union
{
	/*
	 * DRAW_BMP_MODE_VAR_SIZE 模式需要的参数
	 * copy_pt： 需要重复的点的位置
	 */
	UIPoint copy_pt;
	/*
	 * DRAW_BMP_MODE_STRETCH 模式需要的参数
	 * fast == TRUE: 使用邻近取样插值实现，速度比较快，清晰单像素感明显，适用用像素间没有灰度变化的图片放缩（比如二维码）
	 * fast == FALSE: 使用双线性插值实现，速度稍慢，质量好，像素间有过度但有些模糊，适用于有灰度变化的图片放缩（比如OSD图片）
	 */
	BOOL fast;
	/*
	 * DRAW_BMP_MODE_REPEAT_RIGHT 模式需要的参数
	 * start_x: 开始重复的位置
	 */
	int start_x;
	/*
	 * DRAW_BMP_MODE_PART 模式需要的参数
	 * part_rect: 指定需要画的图片部分的位置
	 */
	UIRect part_rect;
	/*
	 * DRAW_BMP_MODE_ROTATE 模式需要的参数
	 * angle: 旋转的角度，-180 ~ 180
	 * w_ratio: 横向放缩比例
	 * h_ratio: 纵向放缩比例
	 */
	RotateBmpParam rotate;
} DrawModeParam;

typedef struct _draw_bmp_param_
{
	UIBmp bmp_info;
	DrawBmpMode mode;
	DrawModeParam mode_param;
} DrawBmpParam;


//-------------------------------------------------------------------------------------------------
// create/destory UIBmp
//-------------------------------------------------------------------------------------------------
/* same color mode with system */
BOOL init_system_bitmap(UIBmp *bmp, UISize size);
/* same color mode with src */
BOOL init_compatible_bitmap(UIBmp *bmp, UIBmp *src, UISize size);
void destroy_bitmap(UIBmp *bmp);


//-------------------------------------------------------------------------------------------------
// UIBmp proccess
//-------------------------------------------------------------------------------------------------
void stretch_bitmap_bilinear(UIBmp* src, UIBmp* dst);


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
int draw_bitmap(UIPainter *painter, UIRect rect, DrawBmpParam *param);



typedef enum
{
	/*NEW BMP START**/
	MAIN_MENU_BAR,
	ASK_BAR,
	RENAME_INPUTBAR,
	RENAME_NORMALBAR,
	RENAME_LIGHTBAR,
	INPUT_NORMAL,
	INPUT_NORMALBAR,
	MENU_LIGHTBAR,
	TP_HEADER,
	PANEL_HEADER,
	PANEL_BOTTOM,
	INSTALL_HEADER,
	SYSTEM_HEADER,
	CH_EDIT_HEADER,
	GAME_HEADER,
	USB_HEADER,
	GRAY_SMALLBAR,
	BLUE_SMALLBAR,
	VOL_OUTBAR,
	VOL_INBAR,
	CH_HEADER,
	CH_BOTTOM,
	SIGNAL_REDBAR,
	SIGNAL_GRAYBAR,
	SIGNAL_YELLOWBAR,
	TIME_BAR,
	WHITE_LINE_BAR,
	TITLE_BAR,
	VOLUM_BAR,
	MAX_BAR
	/*New end */
} bmp_h_bar_type;

typedef enum
{
	SCROLL_BAR,
	MAIN_TOP_BAR,
	SIGNAL_BAR,
	SCROLL_BOX
} bmp_v_bar_type;

typedef enum
{
/*NEW START*/
	CHLIST_OUTPANEL,
	CHLIST_INPANEL,
	PLAYER_PANEL,
	COMMON_PANEL,
	MAIN_MENU_PANEL,
	BLUE_PANEL
} bmp_panel_type;

void draw_bmp_hor_bar(UIPainter *painter, UIPoint point, int length, bmp_h_bar_type type);
void draw_bmp_ver_bar(UIPainter *painter, UIPoint point, int length, bmp_v_bar_type type);
void draw_bmp_panel(UIPainter *painter, UIRect rect, bmp_panel_type type);
#endif /* _GUI_BITMAP_H_ */
