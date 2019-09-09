/*
 * page.c
 *
 *  Created on: 2018-7-6
 *      Author: Brant
 */

#include <gui_window.h>
//#include <guicore/winapi.h>
#include <control/panel.h>


//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// internal use
//-------------------------------------------------------------------------------------------------
static UIPanelData* new_panel_private_data(void)
{
	UIPanelData *panel_data;

	panel_data = (UIPanelData *)gui_malloc(sizeof(UIPanelData));
	if (panel_data == NULL)
	{
		return NULL;
	}

	default_panel_data(panel_data);

	return panel_data;
}


static void delete_panel_private_data(UIPanelData *panel_data)
{
	if (panel_data == NULL)
		return;

	if (panel_data->title)
		gui_free(panel_data->title);

	if (panel_data->container.sizer)
		destroy_sizer(panel_data->container.sizer);

	gui_free(panel_data);
}

static int set_panel_title_internal(UIPanelData *panel_data, const char *text)
{
	int text_len = strlen(text);
	BOOL need_new_space = FALSE;

	if (panel_data->title == NULL || panel_data->title_size <= text_len)
	{
		need_new_space = TRUE;
	}

	if (need_new_space)
	{
		if (panel_data->title != NULL)
		{
			gui_free(panel_data->title);
			panel_data->title_size = 0;
		}
		panel_data->title = (char *)gui_malloc((text_len + 1) * sizeof(U8));
		if (panel_data->title == NULL)
		{
			return -1;
		}
		panel_data->title_size = text_len + 1;
	}

	gui_memset(panel_data->title, 0, panel_data->title_size);
	gui_snprintf(panel_data->title, panel_data->title_size, "%s", text);

	return 0;
}

static UISize calculate_text_size(UIPanelData *panel_data, UIPanelThemeDefaultData *theme_param)
{
	UISize size;

	size.width = get_text_width(panel_data->title, -1, theme_param->font_size, 100);
	size.height = get_text_height(theme_param->font_size, 100);

	return size;
}


//-------------------------------------------------------------------------------------------------
// window manager callback
//-------------------------------------------------------------------------------------------------
static BOOL panel_on_draw(UIPanel *self)
{
    UIPanelThemeDefaultData default_panel;

	default_panel.title_color = RED_COLOR;
	default_panel.title_bkg_color = 0xFF9BCD9B;
	default_panel.border_color = 0xFF9932CC;
	default_panel.inside_bkg_color = 0xFF98F5FF;
	default_panel.border_size = 2;
	default_panel.title_height = 60;
	default_panel.font_size = -1;

	return panel_theme_default(self, &default_panel);
}

static BOOL panel_on_destroy(UIPanel *self)
{
	delete_panel_private_data(self->private_data);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
void default_panel_data(UIPanelData *panel_data)
{
	/* parameter inherits from container */
	default_container_data(&(panel_data->container));

	/* special for panel */
	panel_data->title = NULL;
	panel_data->title_size = 0;
	panel_data->centered = TRUE;
}

UIPanel* create_panel(const char *id)
{
	UIPanel *panel;

	panel = create_sub_window(id, 490, 285, 300, 150);
	if (panel)
	{
        UIPanelData *panel_data = new_panel_private_data();
        if (panel_data == NULL)
		{
			win_destroy(panel);
			return NULL;
		}

		panel->private_data = panel_data;
		panel->focus_stop = TRUE;
		panel->class_name = PANEL_CLASS_NAME;
		panel->on_draw = panel_on_draw;
		panel->on_destroy = panel_on_destroy;
	}

	return panel;
}


int set_panel_title(UIPanel* panel, const char *text)
{
    UIPanelData *panel_data;

	if (!panel || !panel->private_data || !text)
	{
		return -1;
	}

	panel_data = (UIPanelData *)panel->private_data;
	set_panel_title_internal(panel_data, text);
	invalidate_window(panel);

	return 0;
}

int set_panel_inset(UIPanel* panel, UIInset inset)
{
	UIPanelData *panel_data;

	if (!panel || !panel->private_data)
	{
		return -1;
	}

	panel_data = (UIPanelData *)panel->private_data;
	set_container_inset(panel, inset);
	if (panel_data->centered)
		win_set_rect(panel, rect_align(panel->rect.size, ui_rect(0, 0, main_canvas_width(), main_canvas_height()), UI_ALIGN_CENTER));

    return 0;
}

int set_panel_sizer(UIPanel *panel, UISizer *sizer)
{
	UIPanelData *panel_data;

	if (!panel || !panel->private_data)
	{
		return -1;
	}

	panel_data = (UIPanelData *)panel->private_data;
	set_container_sizer(panel, sizer);
	if (panel_data->centered)
		win_set_rect(panel, rect_align(panel->rect.size, ui_rect(0, 0, main_canvas_width(), main_canvas_height()), UI_ALIGN_CENTER));

    return 0;
}

int set_panel_centered(UIPanel *panel, BOOL centered)
{
	UIPanelData *panel_data;

	if (!panel || !panel->private_data)
	{
		return -1;
	}

	panel_data = (UIPanelData *)panel->private_data;
	if (panel_data->centered != centered)
	{
		panel_data->centered = centered;
		if (centered)
			win_set_rect(panel, rect_align(panel->rect.size, ui_rect(0, 0, main_canvas_width(), main_canvas_height()), UI_ALIGN_CENTER));
	}

	return 0;
}


//-------------------------------------------------------------------------------------------------
// add sub window
//-------------------------------------------------------------------------------------------------
BOOL panel_add_window(UIPanel* panel, UIWindow *sub_win)
{
	if ( !panel || !panel->private_data || !sub_win )
	{
		return FALSE;
	}

	//add sub to panel
    win_add_sub(panel, sub_win);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// typical theme
//-------------------------------------------------------------------------------------------------
BOOL panel_theme_default(UIPanel* panel, UIPanelThemeDefaultData *theme_param)
{
    UIPanelData *panel_data = panel->private_data;
	UIPainter *painter = &(panel->painter);
	UIRect text_rect ;
	DrawTextParam param;
	UISize text_size = {0, 0};

	text_size = calculate_text_size(panel_data, theme_param);
	//draw title area
    draw_rect_frame_gui(painter, ui_rect(0, 0, panel->rect.size.width, theme_param->title_height), \
                        theme_param->border_size, theme_param->border_color, theme_param->title_bkg_color);
	//draw title
    default_draw_text_param(&param);
    param.color = theme_param->title_color;
	param.font_size = theme_param->font_size;

	if(panel->rect.size.width > text_size.width)
    {
        text_rect.point.x = (panel->rect.size.width - text_size.width)/2;
        text_rect.size.width = text_size.width;
    }
    else
    {
        text_rect.point.x = 5;
        text_rect.size.width = panel->rect.size.width - 10;
    }

    if( theme_param->title_height > (text_size.height+4) )
    {
        text_rect.point.y = (theme_param->title_height - text_size.height)/2 ;
        text_rect.size.height = text_size.height;
    }
    else
    {
        text_rect.point.y = 2;
        text_rect.size.height = panel->rect.size.height - 4;
    }

    draw_text(painter, text_rect, panel_data->title, -1, &param);

    if(panel_data->container.inset.top < theme_param->title_height)
    {
       panel_data->container.inset.top =  theme_param->title_height + 10;
    }

    draw_rect_frame_gui(painter, ui_rect(0, theme_param->title_height - theme_param->border_size, panel->rect.size.width, panel->rect.size.height - theme_param->title_height), \
                        theme_param->border_size, theme_param->border_color, theme_param->inside_bkg_color);

    return TRUE;
}
