/*
 * popup.c
 *
 *  Created on: 2018-7-6
 *      Author: Brant
 */

#include <gui_window.h>
#include <control/popup.h>


//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// internal use
//-------------------------------------------------------------------------------------------------
static UIPopupData* new_popup_private_data(void)
{
	UIPopupData *popup_data;

	popup_data = (UIPopupData *)gui_malloc(sizeof(UIPopupData));
	if (popup_data == NULL)
	{
		return NULL;
	}

	default_popup_data(popup_data);

	return popup_data;
}

static int set_popup_title_internal(UIPopupData *popup_data, const char *text)
{
	return set_string_text(&(popup_data->title), text);
}

static UISize calculate_text_size(UIPopupData *popup_data, UIPopupThemeDefaultData *theme_param)
{
	UISize size;

	size.width = get_text_width(popup_data->title.content, -1, theme_param->font_size, 100);
	size.height = get_text_height(theme_param->font_size, 100);

	return size;
}

static void center_window_if_needed(UIPopup *popup)
{
	UIPopupData *popup_data;
	UIRect rect;

	if (!popup || !popup->private_data)
		return;

	popup_data = (UIPopupData *)popup->private_data;
	if (!popup_data->centered)
		return;

	rect = rect_align(popup->rect.size, ui_rect(0, 0, main_canvas_width(), main_canvas_height()), UI_ALIGN_CENTER);
	rect.point = screen_to_client(popup->parent, rect.point);
	set_window_rect(popup, rect);
}


//-------------------------------------------------------------------------------------------------
// window manager callback
//-------------------------------------------------------------------------------------------------
/*static */BOOL popup_on_draw(UIPopup *self)
{
    UIPopupThemeDefaultData default_popup;

	default_popup.title_color = RED_COLOR;
	default_popup.title_bkg_color = 0xFF9BCD9B;
	default_popup.border_color = 0xFF9932CC;
	default_popup.inside_bkg_color = 0xFF98F5FF;
	default_popup.border_size = 2;
	default_popup.title_height = 60;
	default_popup.font_size = -1;

	return popup_theme_default(self, &default_popup);
}

static BOOL popup_on_destroy(UIPopup *self)
{
	if (self->private_data)
	{
		delete_popup_data(self->private_data);
		gui_free(self->private_data);
	}

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
void default_popup_data(UIPopupData *popup_data)
{
	/* parameter inherits from container */
	default_container_data(&(popup_data->container));
	popup_data->container.auto_size = TRUE;
	popup_data->container.inset.left = 20;
	popup_data->container.inset.top = 80;
	popup_data->container.inset.right = 20;
	popup_data->container.inset.bottom = 20;

	/* special for popup */
	init_string(&(popup_data->title));
	popup_data->centered = TRUE;
}

void delete_popup_data(UIPopupData *popup_data)
{
	if (popup_data == NULL)
		return;

	delete_container_data(&(popup_data->container));
	destroy_string(&(popup_data->title));
}

UIPopup* create_popup(const char *id)
{
	UIPopup *popup;

	popup = create_sub_window(id, 0, 0, 300, 150);
	if (popup)
	{
        UIPopupData *popup_data = new_popup_private_data();
        if (popup_data == NULL)
		{
			destroy_window(popup);
			return NULL;
		}

		popup->private_data = popup_data;
		popup->focus_stop = TRUE;
		popup->class_name = POPUP_CLASS_NAME;
		popup->on_draw = popup_on_draw;
		popup->on_destroy = popup_on_destroy;
	}

	return popup;
}


int set_popup_title(UIPopup* popup, const char *text)
{
    UIPopupData *popup_data;

	if (!popup || !popup->private_data || !text)
	{
		return -1;
	}

	popup_data = (UIPopupData *)popup->private_data;
	set_popup_title_internal(popup_data, text);
	invalidate_window(popup);

	return 0;
}

int set_popup_inset(UIPopup* popup, UIInset inset)
{
	if (!popup || !popup->private_data)
		return -1;

	set_container_inset(popup, inset);
	center_window_if_needed(popup);

    return 0;
}

int set_popup_sizer(UIPopup *popup, UISizer *sizer)
{
	if (!popup || !popup->private_data)
		return -1;

	set_container_sizer(popup, sizer);
	center_window_if_needed(popup);

    return 0;
}

int set_popup_centered(UIPopup *popup, BOOL centered)
{
	UIPopupData *popup_data;

	if (!popup || !popup->private_data)
	{
		return -1;
	}

	popup_data = (UIPopupData *)popup->private_data;
	if (popup_data->centered != centered)
	{
		popup_data->centered = centered;
		center_window_if_needed(popup);
	}

	return 0;
}


//-------------------------------------------------------------------------------------------------
// add sub window
//-------------------------------------------------------------------------------------------------
BOOL popup_add_window(UIPopup* popup, UIWindow *sub_win)
{
	if (!popup || !popup->private_data || !sub_win)
		return FALSE;

	container_add_sub_window(popup, sub_win);
	center_window_if_needed(popup);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// typical theme
//-------------------------------------------------------------------------------------------------
BOOL popup_theme_default(UIPopup* popup, UIPopupThemeDefaultData *theme_param)
{
    UIPopupData *popup_data = popup->private_data;
	UIPainter *painter = &(popup->painter);
	UIRect text_rect ;
	DrawTextParam param;
	UISize text_size = {0, 0};

	text_size = calculate_text_size(popup_data, theme_param);
	//draw title area
    draw_rect_frame_gui(painter, ui_rect(0, 0, popup->rect.size.width, theme_param->title_height), \
                        theme_param->border_size, theme_param->border_color, theme_param->title_bkg_color);
	//draw title
    default_draw_text_param(&param);
    param.color = theme_param->title_color;
	param.font_size = theme_param->font_size;

	if(popup->rect.size.width > text_size.width)
    {
        text_rect.point.x = (popup->rect.size.width - text_size.width)/2;
        text_rect.size.width = text_size.width;
    }
    else
    {
        text_rect.point.x = 5;
        text_rect.size.width = popup->rect.size.width - 10;
    }

    if( theme_param->title_height > (text_size.height+4) )
    {
        text_rect.point.y = (theme_param->title_height - text_size.height)/2 ;
        text_rect.size.height = text_size.height;
    }
    else
    {
        text_rect.point.y = 2;
        text_rect.size.height = popup->rect.size.height - 4;
    }

    draw_text(painter, text_rect, popup_data->title.content, -1, &param);

    if(popup_data->container.inset.top < theme_param->title_height)
    {
       popup_data->container.inset.top =  theme_param->title_height + 10;
    }

    draw_rect_frame_gui(painter, ui_rect(0, theme_param->title_height - theme_param->border_size, popup->rect.size.width, popup->rect.size.height - theme_param->title_height), \
                        theme_param->border_size, theme_param->border_color, theme_param->inside_bkg_color);

    return TRUE;
}
