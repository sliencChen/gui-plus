/*
 * progress_bar.c
 *
 *  Created on: 2018-9-29
 *      Author: Grace
 */

#include <gui_window.h>
#include <control/progress_bar.h>


//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// internal use
//-------------------------------------------------------------------------------------------------
static UIProgressBarData* new_progress_bar_private_data(void)
{
	UIProgressBarData *pb_data;

	pb_data = (UIProgressBarData *)gui_malloc(sizeof(UIProgressBarData));
	if (pb_data == NULL)
		return NULL;

	/* default parameter */
	gui_memset(pb_data, 0, sizeof(UIProgressBarData));
	pb_data->max_value = 100;
	pb_data->show_percent = TRUE;

	return pb_data;
}

static void delete_progress_bar_private_data(UIProgressBarData *progress_bar_data)
{
	if (progress_bar_data)
	{
		gui_free(progress_bar_data);
	}
}


//-------------------------------------------------------------------------------------------------
// window manager callback
//-------------------------------------------------------------------------------------------------
static BOOL progress_bar_on_draw(UIProgressBar *self)
{
	UIProgressBarThemeRect default_theme;

	default_theme.bar_color = BLACK_COLOR;
	default_theme.bar_text_space = 10;
	default_theme.font_size = -1;
	default_theme.height = 20;
	default_theme.progress_color = RED_COLOR;
	default_theme.text_color = WHITE_COLOR;

	return progress_bar_theme_rect(self, &default_theme);
}

static BOOL progress_bar_on_destroy(UIProgressBar *self)
{
	delete_progress_bar_private_data(self->private_data);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UIProgressBar* create_progress_bar(const char *id)
{
	UIWindow *progress_bar = NULL;

	progress_bar = create_control(id, 0, 0, 300, 30);
	if (progress_bar != NULL)
	{
		UIProgressBarData *progress_bar_data;

		progress_bar_data = new_progress_bar_private_data();
		if (progress_bar_data == NULL)
		{
			gui_free(progress_bar);
			return NULL;
		}

		progress_bar->private_data = progress_bar_data;
		progress_bar->focus_stop = TRUE;
		progress_bar->class_name = PROGRESS_BAR_CLASS_NAME;
		progress_bar->on_draw = progress_bar_on_draw;
		progress_bar->on_destroy = progress_bar_on_destroy;
	}

	return progress_bar;
}

int set_progress_bar_max_value(UIProgressBar *pb, U32 max_value)
{
	UIProgressBarData *pb_data;

	if (pb == NULL || pb->private_data == NULL)
		return -1;

	pb_data = (UIProgressBarData *)pb->private_data;
	if (pb_data->max_value == max_value)
		return 0;

	pb_data->max_value = max_value;
	invalidate_window(pb);

	return 0;
}

int set_progress_bar_value(UIProgressBar *pb, U32 value)
{
	UIProgressBarData *pb_data;

	if (pb == NULL || pb->private_data == NULL)
		return -1;

	pb_data = (UIProgressBarData *)pb->private_data;
	if (pb_data->value == value)
		return 0;

	pb_data->value = value;
	invalidate_window(pb);

	return 0;
}

int set_progress_bar_show_percent(UIProgressBar *pb, BOOL show_percent)
{
	UIProgressBarData *pb_data;

	if (pb == NULL || pb->private_data == NULL)
		return -1;

	pb_data = (UIProgressBarData *)pb->private_data;
	if (pb_data->show_percent == show_percent)
		return 0;

	pb_data->show_percent = show_percent;
	invalidate_window(pb);

	return 0;
}


//-------------------------------------------------------------------------------------------------
// typical theme - internal
//-------------------------------------------------------------------------------------------------
static void draw_percent_text(UIPainter *painter, float percent, UIRect rect, UIColor color, int font_size)
{
	DrawTextParam text_param;
	char percent_text[6];

	default_draw_text_param(&text_param);
	text_param.color = color;
	text_param.font_size = font_size;
	gui_snprintf(percent_text, sizeof(percent_text), "%d%%", (int)((percent * 100) + 0.5));
	draw_text(painter, rect, percent_text, -1, &text_param);
}


//-------------------------------------------------------------------------------------------------
// typical theme
//-------------------------------------------------------------------------------------------------
BOOL progress_bar_theme_rect(UIProgressBar *pb, UIProgressBarThemeRect *theme_param)
{
	UIPainter *painter = &(pb->painter);
	UIRect rect = ui_rect(0, 0, pb->rect.size.width, pb->rect.size.height);
	UIProgressBarData *pb_data = (UIProgressBarData *)pb->private_data;

	UISize bar_size;
	UISize progress_size;
	UISize percent_size;

	float progress;

	if (pb_data == NULL)
		return FALSE;

	progress = (float)pb_data->value / (float)pb_data->max_value;
	if (progress > 1)
		progress = 1;

	bar_size = rect.size;
	if (pb_data->show_percent)
	{
		percent_size.width = get_text_width("100%", -1, theme_param->font_size, -1);
		percent_size.height = get_text_height(theme_param->font_size, -1);
		bar_size.width -= (percent_size.width + theme_param->bar_text_space);
	}
	if (theme_param->height <= 0 || theme_param->height > rect.size.height)
		bar_size.height = rect.size.height;
	else
		bar_size.height = theme_param->height;

	progress_size = bar_size;
	progress_size.width = (int)(bar_size.width * progress);

	draw_rect(painter, rect_align(bar_size, rect, UI_ALIGN_CENTER_VER), theme_param->bar_color);
	draw_rect(painter, rect_align(progress_size, rect, UI_ALIGN_CENTER_VER), theme_param->progress_color);
	if (pb_data->show_percent)
	{
		draw_percent_text(painter, progress, rect_align(percent_size, rect, UI_ALIGN_RIGHT | UI_ALIGN_CENTER_VER), theme_param->text_color, theme_param->font_size);
	}

	return TRUE;
}

BOOL progress_bar_theme_bmp(UIProgressBar *pb, UIProgressBarThemeBmp *theme_param)
{
	UIPainter *painter = &(pb->painter);
	UIRect rect = ui_rect(0, 0, pb->rect.size.width, pb->rect.size.height);
	UIProgressBarData *pb_data = (UIProgressBarData *)pb->private_data;

	UISize bar_size;
	UISize progress_size;
	UISize percent_size;
	UISize anchor_size = {0, 0};

	float progress;
	DrawBmpParam bmp_param;

	if (pb_data == NULL)
		return FALSE;

	progress = (float)pb_data->value / (float)pb_data->max_value;
	if (progress > 1)
		progress = 1;

	bar_size = rect.size;
	if (pb_data->show_percent)
	{
		percent_size.width = get_text_width("100%", -1, theme_param->font_size, -1);
		percent_size.height = get_text_height(theme_param->font_size, -1);
		bar_size.width -= (percent_size.width + theme_param->bar_text_space);
	}
	bar_size.height = get_bmp_icon_height(theme_param->bar_bmp);

	progress_size.width = (int)(bar_size.width * progress);
	progress_size.height = get_bmp_icon_height(theme_param->progress_bmp);

	if (theme_param->anchor_bmp != NULL)
	{
		anchor_size = get_bmp_size(theme_param->anchor_bmp);
		progress_size.width = (int)((bar_size.width - anchor_size.width) * progress);
	}

	if (gui_get_bmp(theme_param->bar_bmp, &(bmp_param.bmp_info)))
	{
		bmp_param.mode = DRAW_BMP_MODE_VAR_SIZE;
		bmp_param.mode_param.copy_pt = ui_point(get_bmp_icon_width(theme_param->bar_bmp) / 2, get_bmp_icon_height(theme_param->bar_bmp) / 2);
		draw_bitmap(painter, rect_align(bar_size, rect, UI_ALIGN_CENTER_VER), &bmp_param);
	}
	if (gui_get_bmp(theme_param->progress_bmp, &(bmp_param.bmp_info)))
	{
		bmp_param.mode = DRAW_BMP_MODE_VAR_SIZE;
		bmp_param.mode_param.copy_pt = ui_point(get_bmp_icon_width(theme_param->progress_bmp) / 2, get_bmp_icon_height(theme_param->progress_bmp) / 2);
		draw_bitmap(painter, rect_align(progress_size, rect, UI_ALIGN_CENTER_VER), &bmp_param);
	}
	if (theme_param->anchor_bmp != NULL && gui_get_bmp(theme_param->anchor_bmp, &(bmp_param.bmp_info)))
	{
		UIRect anchor_rect;

		anchor_rect = rect_align(anchor_size, rect, UI_ALIGN_CENTER_VER);
		anchor_rect.point.x = progress_size.width;
		bmp_param.mode = DRAW_BMP_MODE_NORMAL;
		draw_bitmap(painter, anchor_rect, &bmp_param);
	}

	if (pb_data->show_percent)
	{
		draw_percent_text(painter, progress, rect_align(percent_size, rect, UI_ALIGN_RIGHT | UI_ALIGN_CENTER_VER), theme_param->text_color, theme_param->font_size);
	}

	return TRUE;
}
