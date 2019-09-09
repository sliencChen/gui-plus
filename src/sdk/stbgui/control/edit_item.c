/*
 * edit_item.c
 *
 *  Created on: 2019-7-23
 *      Author: Silence
 */

#include <control/edit_item.h>
#include <gui_window.h>


//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// internal use
//-------------------------------------------------------------------------------------------------
static UIEditItemData* new_edit_item_private_data(void)
{
	UIEditItemData *edit_data;

	edit_data = (UIEditItemData *)gui_malloc(sizeof(UIEditItemData));
	if (edit_data == NULL)
	{
		return NULL;
	}

	/* default parameter */
	gui_memset(edit_data, 0, sizeof(UIEditItemData));
	return edit_data;
}


static void delete_edit_item_private_data(UIEditItemData *edit_data)
{
	if (edit_data)
	{
		if (edit_data->field_type == EDIT_FIELD_TYPE_SELECT)
		{
			if(edit_data->edit_field.select_field.select_data.content != NULL)
			{
				gui_free(edit_data->edit_field.select_field.select_data.content);
			}
		}
		else if (edit_data->field_type == EDIT_FIELD_TYPE_INPUT)
		{
			if(edit_data->edit_field.input_field.input_data.content != NULL)
			{
				gui_free(edit_data->edit_field.input_field.input_data.content);
			}
		}
		gui_free(edit_data);
	}
}


static void delete_edit_item_header_skin_data(UIEditItemThemeBmp *theme_data)
{
	if(theme_data->header_skin.header_light_left_icon)
	{
		gui_free(theme_data->header_skin.header_light_left_icon);
	}
	if(theme_data->header_skin.header_light_mid_icon)
	{
		gui_free(theme_data->header_skin.header_light_mid_icon);
	}
	if(theme_data->header_skin.header_light_righ_icon)
	{
		gui_free(theme_data->header_skin.header_light_righ_icon);
	}

	if(theme_data->header_skin.header_normal_left_icon)
	{
		gui_free(theme_data->header_skin.header_normal_left_icon);
	}
	if(theme_data->header_skin.header_normal_mid_icon)
	{
		gui_free(theme_data->header_skin.header_normal_mid_icon);
	}
	if(theme_data->header_skin.header_normal_righ_icon)
	{
		gui_free(theme_data->header_skin.header_normal_righ_icon);
	}
}


static void delete_edit_item_field_skin_data(UIEditItemThemeBmp *theme_data)
{
	if(theme_data->field_skin.field_light_left_icon)
	{
		gui_free(theme_data->field_skin.field_light_left_icon);
	}
	if(theme_data->field_skin.field_light_mid_icon)
	{
		gui_free(theme_data->field_skin.field_light_mid_icon);
	}
	if(theme_data->field_skin.field_light_righ_icon)
	{
		gui_free(theme_data->field_skin.field_light_righ_icon);
	}

	if(theme_data->field_skin.field_normal_left_icon)
	{
		gui_free(theme_data->field_skin.field_normal_left_icon);
	}
	if(theme_data->field_skin.field_normal_mid_icon)
	{
		gui_free(theme_data->field_skin.field_normal_mid_icon);
	}
	if(theme_data->field_skin.field_normal_righ_icon)
	{
		gui_free(theme_data->field_skin.field_normal_righ_icon);
	}
}


static void delete_edit_item_field_inset_skin_data(UIEditItemThemeBmp *theme_data)
{
	if(theme_data->field_skin.inset_type == EDIT_FIELD_INSET_TYPE_BMP)
	{
		if(theme_data->field_skin.edit_field_inset.inset_skin.field_light_inset_left_icon)
		{
			gui_free(theme_data->field_skin.edit_field_inset.inset_skin.field_light_inset_left_icon);
		}
		if(theme_data->field_skin.edit_field_inset.inset_skin.field_light_inset_mid_icon)
		{
			gui_free(theme_data->field_skin.edit_field_inset.inset_skin.field_light_inset_mid_icon);
		}
		if(theme_data->field_skin.edit_field_inset.inset_skin.field_light_inset_righ_icon)
		{
			gui_free(theme_data->field_skin.edit_field_inset.inset_skin.field_light_inset_righ_icon);
		}

		if(theme_data->field_skin.edit_field_inset.inset_skin.field_normal_inset_left_icon)
		{
			gui_free(theme_data->field_skin.edit_field_inset.inset_skin.field_normal_inset_left_icon);
		}
		if(theme_data->field_skin.edit_field_inset.inset_skin.field_normal_inset_mid_icon)
		{
			gui_free(theme_data->field_skin.edit_field_inset.inset_skin.field_normal_inset_mid_icon);
		}
		if(theme_data->field_skin.edit_field_inset.inset_skin.field_normal_inset_righ_icon)
		{
			gui_free(theme_data->field_skin.edit_field_inset.inset_skin.field_normal_inset_righ_icon);
		}
	}
}


static void delete_edit_item_theme_data(UIEditItem *self)
{
	UIEditItemThemeBmp *theme_data = (UIEditItemThemeBmp *)self->theme_data;
	if(theme_data)
	{
		delete_edit_item_header_skin_data(theme_data);
		delete_edit_item_field_skin_data(theme_data);
		delete_edit_item_field_inset_skin_data(theme_data);
	}
}


static int set_edit_item_header_internal(UIEditItemData *edit_data, const char *text)
{
	return set_string_text(&edit_data->header, text);
}


static int set_edit_item_field_internal(UIEditItemData *edit_data, const char *text)
{
	int ret = -1;
	switch(edit_data->field_type)
	{
		case EDIT_FIELD_TYPE_SELECT:
			ret = set_string_text(&edit_data->edit_field.select_field.select_data, text);
		break;
		case EDIT_FIELD_TYPE_INPUT:
			ret = set_string_text(&edit_data->edit_field.input_field.input_data, text);
		break;
	}
	return ret;
}


static BOOL is_draw_header_light_icon_VAR(EditItemHeaderSkin *header_skin)
{
	if(header_skin->header_light_left_icon \
			&& (header_skin->header_light_left_icon == header_skin->header_light_mid_icon) \
			&& (header_skin->header_light_left_icon == header_skin->header_light_righ_icon) \
			&& (header_skin->header_light_mid_icon == header_skin->header_light_righ_icon))
	{
		return TRUE;
	}
	return FALSE;
}


static BOOL is_draw_header_normal_icon_VAR(EditItemHeaderSkin *header_skin)
{
	if(header_skin->header_normal_left_icon \
			&& (header_skin->header_normal_left_icon == header_skin->header_normal_mid_icon) \
			&& (header_skin->header_normal_left_icon == header_skin->header_normal_righ_icon) \
			&& (header_skin->header_normal_mid_icon == header_skin->header_normal_righ_icon))
	{
		return TRUE;
	}
	return FALSE;
}


static BOOL is_draw_field_light_icon_VAR(EditItemFieldSkin *field_skin)
{

	if(field_skin->field_light_left_icon \
			&& (field_skin->field_light_left_icon == field_skin->field_light_mid_icon) \
			&& (field_skin->field_light_left_icon == field_skin->field_light_righ_icon) \
			&& (field_skin->field_light_mid_icon == field_skin->field_light_righ_icon))
	{
		return TRUE;
	}
	return FALSE;
}


static BOOL is_draw_field_normal_icon_VAR(EditItemFieldSkin *field_skin)
{
	if(field_skin->field_normal_left_icon \
			&& (field_skin->field_normal_left_icon == field_skin->field_normal_mid_icon) \
			&& (field_skin->field_normal_left_icon == field_skin->field_normal_righ_icon) \
			&& (field_skin->field_normal_mid_icon == field_skin->field_normal_righ_icon))
	{
		return TRUE;
	}
	return FALSE;
}


static BOOL is_draw_edit_item_light_skin_VAR(EditItemHeaderSkin *header_skin, EditItemFieldSkin *field_skin)
{
	if(is_draw_header_light_icon_VAR(header_skin) && is_draw_field_light_icon_VAR(field_skin) \
			&& (header_skin->header_light_left_icon == field_skin->field_light_left_icon))
	{
		return TRUE;
	}
	return FALSE;
}


static BOOL is_draw_edit_item_normal_skin_VAR(EditItemHeaderSkin *header_skin, EditItemFieldSkin *field_skin)
{
	if(is_draw_header_normal_icon_VAR(header_skin) && is_draw_field_normal_icon_VAR(field_skin) \
			&& (header_skin->header_normal_left_icon == field_skin->field_normal_left_icon))
	{
		return TRUE;
	}
	return FALSE;
}


static BOOL is_draw_field_inset_light_icon_VAR(EditItemFieldInsetSkin *inset_skin)
{
	if(inset_skin->field_light_inset_left_icon \
				&& (inset_skin->field_light_inset_left_icon == inset_skin->field_light_inset_righ_icon) \
				&& (inset_skin->field_light_inset_left_icon == inset_skin->field_light_inset_mid_icon) \
				&& (inset_skin->field_light_inset_mid_icon == inset_skin->field_light_inset_righ_icon))
	{
		return TRUE;
	}
	return FALSE;
}


static BOOL is_draw_field_inset_normal_icon_VAR(EditItemFieldInsetSkin *inset_skin)
{
	if(inset_skin->field_normal_inset_left_icon \
				&& (inset_skin->field_normal_inset_left_icon == inset_skin->field_normal_inset_righ_icon) \
				&& (inset_skin->field_normal_inset_left_icon == inset_skin->field_normal_inset_mid_icon) \
				&& (inset_skin->field_normal_inset_mid_icon == inset_skin->field_normal_inset_righ_icon))
	{
		return TRUE;
	}
	return FALSE;
}


//-------------------------------------------------------------------------------------------------
// class event
//-------------------------------------------------------------------------------------------------
static int edit_item_on_select_key(struct _ui_window_ *window, Event event, EventData event_data)
{
	UIPage *page = (UIPage *)event_data;
	UIEditItemData *edit_item_data = (UIEditItemData *)window->private_data;
	if(edit_item_data->edit_field.select_field.select_action)
	{
		edit_item_data->edit_field.select_field.select_action(page, window);
	}
	return 0;
};


BOOL register_edit_item_event(void)
{
	BOOL result = TRUE;

	result &= register_class_event_handler(EDIT_ITEM_CLASS_NAME, GUI_EVT_VK_UP, 0);
	result &= register_class_event_handler(EDIT_ITEM_CLASS_NAME, GUI_EVT_VK_DOWN, 0);
	result &= register_class_event_handler(EDIT_ITEM_CLASS_NAME, GUI_EVT_VK_SELECT, edit_item_on_select_key);
	return result;
}


static BOOL register_edit_item_class(void)
{
	static BOOL registered = FALSE;
	BOOL res;

	if (registered) return FALSE;

	res = register_edit_item_event();
	registered = TRUE;

	return res;
}


//-------------------------------------------------------------------------------------------------
// window manager callback
//-------------------------------------------------------------------------------------------------



BOOL edit_item_theme_rect(UIEditItem *self, UIEditItemThemeDefault *theme_param)
{
	UIEditItemData *edit_data = (UIEditItemData *)self->private_data;
	if (edit_data == NULL)
	{
		return FALSE;
	}

	int left_inset_width = 0;
	int right_inset_width = 0;
	DrawTextParam text_param;
	UIRect text_rect;
	UIColor header_bg_color;
	UIColor field_bg_color;
	char *field_data;
	default_draw_text_param(&text_param);

	text_param.font_size = theme_param->edit_default_rect.font_size;
	UIRect rect = ui_rect(0, 0, self->rect.size.width, self->rect.size.height);
	UIRect header_rect = ui_rect(0, 0, edit_data->header_width, rect.size.height);
	UIRect field_rect = ui_rect(edit_data->header_width, 0, rect.size.width - edit_data->header_width, rect.size.height);
	UIPainter *painter = &(self->painter);

	if(edit_data->high_light)
	{
		header_bg_color = theme_param->edit_default_rect.header_bkg_light_color;
		field_bg_color = theme_param->edit_default_rect.field_bkg_light_color;
	}
	else
	{
		header_bg_color = theme_param->edit_default_rect.header_bkg_normal_color;
		field_bg_color = theme_param->edit_default_rect.field_bkg_normal_color;
	}

	if(edit_data->field_type == EDIT_FIELD_TYPE_SELECT)
	{
		field_data = edit_data->edit_field.select_field.select_data.content;
	}
	else if(edit_data->field_type == EDIT_FIELD_TYPE_INPUT)
	{
		field_data = edit_data->edit_field.input_field.input_data.content;
	}

	/*draw header*/
	if(edit_data->high_light)
	{
		text_param.color = theme_param->edit_default_rect.header_light_color;
	}
	else
	{
		text_param.color = theme_param->edit_default_rect.header_normal_color;
	}
	draw_rect(painter, header_rect, header_bg_color);

   	text_rect.size.width = get_text_width(edit_data->header.content, -1, text_param.font_size, -1);
   	text_rect.size.height = get_text_height(text_param.font_size, -1);

   	if(theme_param->edit_default_rect.header_centered)
   	{
   		text_rect = rect_align(text_rect.size,header_rect, UI_ALIGN_CENTER);
   	}
   	else
   	{
   		text_rect = rect_align(text_rect.size,header_rect, UI_ALIGN_CENTER_VER);
		if(theme_param->edit_default_rect.header_margin.left + text_rect.size.width > header_rect.size.width)
		{
			theme_param->edit_default_rect.header_margin.left = header_rect.size.width - text_rect.size.width;
		}
		text_rect.point.x = header_rect.point.x + theme_param->edit_default_rect.header_margin.left;
   	}
	draw_text(painter, text_rect, edit_data->header.content, -1, &text_param);

	/*draw data field*/
	if(edit_data->high_light)
	{
		text_param.color = theme_param->edit_default_rect.field_light_color;
	}
	else
	{
		text_param.color = theme_param->edit_default_rect.field_normal_color;
	}
	draw_rect(painter, field_rect, field_bg_color);

	if(theme_param->inset_graphic_draw.inset_graphic_on_draw)
	{
		theme_param->inset_graphic_draw.inset_graphic_on_draw(self,(void *)theme_param);
	}

	if(edit_data->high_light)
	{
		left_inset_width = theme_param->inset_graphic_draw.inset_light_graphic_width \
				+ theme_param->inset_graphic_draw.field_light_inset_left_graphic_margin.left \
				+ theme_param->inset_graphic_draw.field_light_inset_left_graphic_margin.right;
		right_inset_width = theme_param->inset_graphic_draw.inset_light_graphic_width \
				+ theme_param->inset_graphic_draw.field_light_inset_righ_graphic_margin.left \
				+ theme_param->inset_graphic_draw.field_light_inset_righ_graphic_margin.right;
	}
	else
	{
		left_inset_width = theme_param->inset_graphic_draw.inset_normal_graphic_width \
				+ theme_param->inset_graphic_draw.field_normal_inset_left_graphic_margin.left \
				+ theme_param->inset_graphic_draw.field_normal_inset_left_graphic_margin.right;
		right_inset_width = theme_param->inset_graphic_draw.inset_normal_graphic_width \
				+ theme_param->inset_graphic_draw.field_normal_inset_righ_graphic_margin.left \
				+ theme_param->inset_graphic_draw.field_normal_inset_righ_graphic_margin.right;
	}

   	text_rect.size.width = get_text_width(field_data, -1, text_param.font_size, -1);
   	text_rect.size.height = get_text_height(text_param.font_size, -1);

	if(theme_param->edit_default_rect.field_centered)
	{
		text_rect = rect_align(text_rect.size, field_rect, UI_ALIGN_CENTER);
	}
	else
	{
		text_rect = rect_align(text_rect.size, field_rect, UI_ALIGN_CENTER_VER);

		if(theme_param->edit_default_rect.field_margin.left < left_inset_width)
		{
			theme_param->edit_default_rect.field_margin.left = left_inset_width;
		}

		if(theme_param->edit_default_rect.field_margin.left + text_rect.size.width + right_inset_width > field_rect.size.width)
		{
			theme_param->edit_default_rect.field_margin.left = field_rect.size.width - text_rect.size.width - right_inset_width;
		}
		text_rect.point.x = field_rect.point.x + theme_param->edit_default_rect.field_margin.left;
	}
	draw_text(painter,text_rect,field_data, -1, &text_param);
	return TRUE;
}


BOOL edit_item_theme_bmp(UIEditItem *self, UIEditItemThemeBmp *theme_param)
{
	UIEditItemData *edit_data = (UIEditItemData *)self->private_data;
	if (edit_data == NULL)
	{
		return FALSE;
	}

	UIBmp bmp_info;
	DrawBmpParam bmp_param;
	DrawTextParam text_param;

	UIRect left_icon_rect;
	UIRect mid_icon_rect;
	UIRect right_icon_rect;

	/*inset*/
	UIRect left_inset_icon_rect;
	UIRect mid_inset_icon_rect;
	UIRect right_inset_icon_rect;
	int left_inset_width = 0;
	int right_inset_width = 0;

	BOOL is_draw_item_light_skin_var = FALSE;
	BOOL is_draw_item_normal_skin_var = FALSE;
	UIRect text_rect;
	UIColor header_bg_color;
	UIColor field_bg_color;
	char *field_data;
	default_draw_text_param(&text_param);

	text_param.font_size = theme_param->edit_theme_rect.font_size;
	UIRect rect = ui_rect(0, 0, self->rect.size.width, self->rect.size.height);
	UIRect header_rect = ui_rect(0, 0, edit_data->header_width, rect.size.height);
	UIRect header_bg_rect = header_rect;
	UIRect field_rect = ui_rect(edit_data->header_width, 0, rect.size.width - edit_data->header_width, rect.size.height);
	UIRect field_bg_rect = field_rect;
	UIPainter *painter = &(self->painter);

	if(edit_data->high_light)
	{
		header_bg_color = theme_param->edit_theme_rect.header_bkg_light_color;
		field_bg_color = theme_param->edit_theme_rect.field_bkg_light_color;
	}
	else
	{
		header_bg_color = theme_param->edit_theme_rect.header_bkg_normal_color;
		field_bg_color = theme_param->edit_theme_rect.field_bkg_normal_color;
	}

	if(edit_data->field_type == EDIT_FIELD_TYPE_SELECT)
	{
		field_data = edit_data->edit_field.select_field.select_data.content;
	}
	else if(edit_data->field_type == EDIT_FIELD_TYPE_INPUT)
	{
		field_data = edit_data->edit_field.input_field.input_data.content;
	}


	if(edit_data->high_light)
	{
		if(is_draw_edit_item_light_skin_VAR(&theme_param->header_skin,&theme_param->field_skin))
		{
			is_draw_item_light_skin_var = TRUE;
			draw_rect(painter, header_bg_rect, header_bg_color);
			draw_rect(painter, field_bg_rect, field_bg_color);
			bmp_param.mode = DRAW_BMP_MODE_VAR_SIZE;
			gui_get_bmp(theme_param->header_skin.header_light_left_icon, &bmp_info);
			bmp_param.bmp_info = bmp_info;
			bmp_param.mode_param.copy_pt = ui_point(get_bmp_icon_width(theme_param->header_skin.header_light_left_icon) / 2, get_bmp_icon_height(theme_param->header_skin.header_light_left_icon) / 2);
			left_icon_rect = ui_rect(0, 0, rect.size.width, bmp_info.pixel_buf.size.height);
			draw_bitmap(painter, left_icon_rect, &bmp_param);
		}
	}
	else
	{
		if(is_draw_edit_item_normal_skin_VAR(&theme_param->header_skin,&theme_param->field_skin))
		{
			is_draw_item_normal_skin_var = TRUE;
			draw_rect(painter, header_bg_rect, header_bg_color);
			draw_rect(painter, field_bg_rect, field_bg_color);
			bmp_param.mode = DRAW_BMP_MODE_VAR_SIZE;
			gui_get_bmp(theme_param->header_skin.header_normal_left_icon, &bmp_info);
			bmp_param.bmp_info = bmp_info;
			bmp_param.mode_param.copy_pt = ui_point(get_bmp_icon_width(theme_param->header_skin.header_normal_left_icon) / 2, get_bmp_icon_height(theme_param->header_skin.header_normal_left_icon) / 2);
			left_icon_rect = ui_rect(0, 0, rect.size.width, bmp_info.pixel_buf.size.height);
			draw_bitmap(painter, left_icon_rect, &bmp_param);
		}
	}

	/*draw header*/
	gui_memset(&left_icon_rect,0,sizeof(left_icon_rect));
	gui_memset(&mid_icon_rect,0,sizeof(mid_icon_rect));
	gui_memset(&right_icon_rect,0,sizeof(right_icon_rect));
	if(edit_data->high_light && FALSE == is_draw_item_light_skin_var)
	{
		/*draw bg color*/
		if(theme_param->header_skin.header_light_left_icon)
		{
			gui_get_bmp(theme_param->header_skin.header_light_left_icon,&bmp_info);
			header_bg_rect.point.x  = bmp_info.pixel_buf.size.width;
		}

		if(theme_param->header_skin.header_light_righ_icon)
		{
			gui_get_bmp(theme_param->header_skin.header_light_righ_icon,&bmp_info);
			header_bg_rect.size.width  -= bmp_info.pixel_buf.size.width;
		}
		draw_rect(painter, header_bg_rect, header_bg_color);
		/*var mode*/
		if(is_draw_header_light_icon_VAR(&theme_param->header_skin))
		{
			bmp_param.mode = DRAW_BMP_MODE_VAR_SIZE;
			gui_get_bmp(theme_param->header_skin.header_light_left_icon, &bmp_info);
			bmp_param.bmp_info = bmp_info;
			bmp_param.mode_param.copy_pt = ui_point(get_bmp_icon_width(theme_param->header_skin.header_light_left_icon) / 2, get_bmp_icon_height(theme_param->header_skin.header_light_left_icon) / 2);
			left_icon_rect = ui_rect(0, 0, header_rect.size.width, bmp_info.pixel_buf.size.height);
			draw_bitmap(painter, left_icon_rect, &bmp_param);
		}
		else
		{
			/*left icon*/
			if(theme_param->header_skin.header_light_left_icon)
			{
				bmp_param.mode = DRAW_BMP_MODE_NORMAL;
				gui_get_bmp(theme_param->header_skin.header_light_left_icon,&bmp_info);
				left_icon_rect = ui_rect(0,0,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
				bmp_param.bmp_info = bmp_info;
				draw_bitmap(painter, left_icon_rect, &bmp_param);
			}
			/*right icon*/
			if(theme_param->header_skin.header_light_righ_icon)
			{
				bmp_param.mode = DRAW_BMP_MODE_NORMAL;
				gui_get_bmp(theme_param->header_skin.header_light_righ_icon,&bmp_info);
				right_icon_rect = ui_rect(header_rect.size.width - bmp_info.pixel_buf.size.width,0,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
				bmp_param.bmp_info = bmp_info;
				draw_bitmap(painter, right_icon_rect, &bmp_param);

			}
			/*mid icon*/
			if(theme_param->header_skin.header_light_mid_icon)
			{
				gui_get_bmp(theme_param->header_skin.header_light_mid_icon,&bmp_info);
				bmp_param.mode = DRAW_BMP_MODE_VAR_SIZE;
				bmp_param.bmp_info = bmp_info;
				bmp_param.mode_param.copy_pt = ui_point(get_bmp_icon_width(theme_param->header_skin.header_light_mid_icon) / 2, get_bmp_icon_height(theme_param->header_skin.header_light_mid_icon) / 2);
				mid_icon_rect = ui_rect(left_icon_rect.size.width,0,header_rect.size.width - left_icon_rect.size.width - right_icon_rect.size.width,bmp_info.pixel_buf.size.height);
				draw_bitmap(painter, mid_icon_rect, &bmp_param);
			}
		}
	}
	else if(FALSE == edit_data->high_light && FALSE == is_draw_item_normal_skin_var)/*normal*/
	{
		/*draw bg color*/
		if(theme_param->header_skin.header_normal_left_icon)
		{
			gui_get_bmp(theme_param->header_skin.header_normal_left_icon,&bmp_info);
			header_bg_rect.point.x  = bmp_info.pixel_buf.size.width;
		}

		if(theme_param->header_skin.header_normal_righ_icon)
		{
			gui_get_bmp(theme_param->header_skin.header_normal_righ_icon,&bmp_info);
			header_bg_rect.size.width  -= bmp_info.pixel_buf.size.width;
		}
		draw_rect(painter, header_bg_rect, header_bg_color);
		/*var mode*/
		if(is_draw_header_normal_icon_VAR(&theme_param->header_skin))
		{
			bmp_param.mode = DRAW_BMP_MODE_VAR_SIZE;
			gui_get_bmp(theme_param->header_skin.header_normal_left_icon, &bmp_info);
			bmp_param.bmp_info = bmp_info;
			bmp_param.mode_param.copy_pt = ui_point(get_bmp_icon_width(theme_param->header_skin.header_normal_left_icon) / 2, get_bmp_icon_height(theme_param->header_skin.header_normal_left_icon) / 2);
			left_icon_rect = ui_rect(0, 0, header_rect.size.width, bmp_info.pixel_buf.size.height);
			draw_bitmap(painter, left_icon_rect, &bmp_param);
		}
		else
		{
			/*left icon*/
			if(theme_param->header_skin.header_normal_left_icon)
			{
				bmp_param.mode = DRAW_BMP_MODE_NORMAL;
				gui_get_bmp(theme_param->header_skin.header_normal_left_icon,&bmp_info);
				left_icon_rect = ui_rect(0,0,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
				bmp_param.bmp_info = bmp_info;
				draw_bitmap(painter, left_icon_rect, &bmp_param);
			}
			/*right icon*/
			if(theme_param->header_skin.header_normal_righ_icon)
			{
				bmp_param.mode = DRAW_BMP_MODE_NORMAL;
				gui_get_bmp(theme_param->header_skin.header_normal_righ_icon,&bmp_info);
				right_icon_rect = ui_rect(header_rect.size.width - bmp_info.pixel_buf.size.width,0,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
				bmp_param.bmp_info = bmp_info;
				draw_bitmap(painter, right_icon_rect, &bmp_param);
			}
			/*mid icon*/
			if(theme_param->header_skin.header_normal_mid_icon)
			{
				gui_get_bmp(theme_param->header_skin.header_normal_mid_icon,&bmp_info);
				bmp_param.mode = DRAW_BMP_MODE_VAR_SIZE;
				bmp_param.bmp_info = bmp_info;
				bmp_param.mode_param.copy_pt = ui_point(get_bmp_icon_width(theme_param->header_skin.header_normal_mid_icon) / 2, get_bmp_icon_height(theme_param->header_skin.header_normal_mid_icon) / 2);
				mid_icon_rect = ui_rect(left_icon_rect.size.width,0,header_rect.size.width - left_icon_rect.size.width - right_icon_rect.size.width,bmp_info.pixel_buf.size.height);
				draw_bitmap(painter, mid_icon_rect, &bmp_param);
			}
		}
	}

	/*draw header text*/
	if(edit_data->high_light)
	{
		text_param.color = theme_param->edit_theme_rect.header_light_color;
	}
	else
	{
		text_param.color = theme_param->edit_theme_rect.header_normal_color;
	}

	text_rect.size.width = get_text_width(edit_data->header.content, -1, text_param.font_size, -1);
	text_rect.size.height = get_text_height(text_param.font_size, -1);
	if(theme_param->edit_theme_rect.header_centered)
	{
		text_rect = rect_align(text_rect.size,header_rect, UI_ALIGN_CENTER);
	}
	else
	{
		text_rect = rect_align(text_rect.size,header_rect, UI_ALIGN_CENTER_VER);

		if(theme_param->edit_theme_rect.header_margin.left < left_icon_rect.size.width)
		{
			theme_param->edit_theme_rect.header_margin.left = left_icon_rect.size.width;
		}

		if(theme_param->edit_theme_rect.header_margin.left + text_rect.size.width + right_icon_rect.size.width > header_rect.size.width)
		{
			theme_param->edit_theme_rect.header_margin.left = header_rect.size.width - text_rect.size.width - right_icon_rect.size.width;
		}
		text_rect.point.x = header_rect.point.x + theme_param->edit_theme_rect.header_margin.left;
	}
	draw_text(painter, text_rect, edit_data->header.content, -1, &text_param);

	/*draw data field*/
	gui_memset(&left_icon_rect,0,sizeof(left_icon_rect));
	gui_memset(&mid_icon_rect,0,sizeof(mid_icon_rect));
	gui_memset(&right_icon_rect,0,sizeof(right_icon_rect));
	if(edit_data->high_light && FALSE == is_draw_item_light_skin_var)
	{

		if(theme_param->field_skin.field_light_left_icon)
		{
			gui_get_bmp(theme_param->field_skin.field_light_left_icon,&bmp_info);
			field_bg_rect.point.x = bmp_info.pixel_buf.size.width;
		}

		if(theme_param->field_skin.field_light_righ_icon)
		{
			gui_get_bmp(theme_param->field_skin.field_light_righ_icon,&bmp_info);
			field_bg_rect.size.width -= bmp_info.pixel_buf.size.width;
		}
		draw_rect(painter, field_bg_rect, field_bg_color);
		/*var mode*/
		if(is_draw_field_light_icon_VAR(&theme_param->field_skin))
		{
			bmp_param.mode = DRAW_BMP_MODE_VAR_SIZE;
			gui_get_bmp(theme_param->field_skin.field_light_left_icon, &bmp_info);
			bmp_param.bmp_info = bmp_info;
			bmp_param.mode_param.copy_pt = ui_point(get_bmp_icon_width(theme_param->field_skin.field_light_left_icon) / 2, get_bmp_icon_height(theme_param->field_skin.field_light_left_icon) / 2);
			left_icon_rect = ui_rect(edit_data->header_width, 0, field_rect.size.width, bmp_info.pixel_buf.size.height);
			draw_bitmap(painter, left_icon_rect, &bmp_param);
		}/*nomal mode*/
		else
		{	/*left icon*/
			if(theme_param->field_skin.field_light_left_icon)
			{
				bmp_param.mode = DRAW_BMP_MODE_NORMAL;
				gui_get_bmp(theme_param->field_skin.field_light_left_icon,&bmp_info);
				left_icon_rect = ui_rect(edit_data->header_width, 0, bmp_info.pixel_buf.size.width, bmp_info.pixel_buf.size.height);
				bmp_param.bmp_info = bmp_info;
				draw_bitmap(painter, left_icon_rect, &bmp_param);
			}
			/*right icon*/
			if(theme_param->field_skin.field_light_righ_icon)
			{
				bmp_param.mode = DRAW_BMP_MODE_NORMAL;
				gui_get_bmp(theme_param->field_skin.field_light_righ_icon,&bmp_info);
				right_icon_rect = ui_rect(rect.size.width - bmp_info.pixel_buf.size.width,0,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
				bmp_param.bmp_info = bmp_info;
				draw_bitmap(painter, right_icon_rect, &bmp_param);
			}
			/*mid icon*/
			if(theme_param->field_skin.field_light_mid_icon)
			{
				gui_get_bmp(theme_param->field_skin.field_light_mid_icon,&bmp_info);
				bmp_param.bmp_info = bmp_info;
				bmp_param.mode = DRAW_BMP_MODE_VAR_SIZE;
				bmp_param.mode_param.copy_pt = ui_point(get_bmp_icon_width(theme_param->field_skin.field_light_mid_icon) / 2, get_bmp_icon_height(theme_param->field_skin.field_light_mid_icon) / 2);
				mid_icon_rect = ui_rect(header_rect.size.width + left_icon_rect.size.width,0,field_rect.size.width - left_icon_rect.size.width - right_icon_rect.size.width,bmp_info.pixel_buf.size.height);
				draw_bitmap(painter, mid_icon_rect, &bmp_param);
			}
		}
	}
	else if(FALSE == edit_data->high_light && FALSE == is_draw_item_normal_skin_var)/*normal*/
	{
		if(theme_param->field_skin.field_normal_left_icon)
		{
			gui_get_bmp(theme_param->field_skin.field_normal_left_icon,&bmp_info);
			field_bg_rect.point.x = bmp_info.pixel_buf.size.width;
		}

		if(theme_param->field_skin.field_normal_righ_icon)
		{
			gui_get_bmp(theme_param->field_skin.field_normal_righ_icon,&bmp_info);
			field_bg_rect.size.width  -= bmp_info.pixel_buf.size.width;
		}
		draw_rect(painter, field_bg_rect, field_bg_color);
		if(is_draw_field_normal_icon_VAR(&theme_param->field_skin))
		{
			bmp_param.mode = DRAW_BMP_MODE_VAR_SIZE;
			gui_get_bmp(theme_param->field_skin.field_normal_left_icon, &bmp_info);
			bmp_param.bmp_info = bmp_info;
			bmp_param.mode_param.copy_pt = ui_point(get_bmp_icon_width(theme_param->field_skin.field_normal_left_icon) / 2, get_bmp_icon_height(theme_param->field_skin.field_normal_left_icon) / 2);
			left_icon_rect = ui_rect(edit_data->header_width, 0, field_rect.size.width, bmp_info.pixel_buf.size.height);
			draw_bitmap(painter, left_icon_rect, &bmp_param);
		}/*nomal mode*/
		else
		{	/*left icon*/
			if(theme_param->field_skin.field_normal_left_icon)
			{
				bmp_param.mode = DRAW_BMP_MODE_NORMAL;
				gui_get_bmp(theme_param->field_skin.field_normal_left_icon,&bmp_info);
				left_icon_rect = ui_rect(edit_data->header_width,0,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
				bmp_param.bmp_info = bmp_info;
				draw_bitmap(painter, left_icon_rect, &bmp_param);
			}
			/*right icon*/
			if(theme_param->field_skin.field_normal_righ_icon)
			{
				bmp_param.mode = DRAW_BMP_MODE_NORMAL;
				gui_get_bmp(theme_param->field_skin.field_normal_righ_icon,&bmp_info);
				right_icon_rect = ui_rect(rect.size.width - bmp_info.pixel_buf.size.width,0,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
				bmp_param.bmp_info = bmp_info;
				draw_bitmap(painter, right_icon_rect, &bmp_param);
			}
			/*mid icon*/
			if(theme_param->field_skin.field_normal_mid_icon)
			{
				gui_get_bmp(theme_param->field_skin.field_normal_mid_icon,&bmp_info);
				bmp_param.bmp_info = bmp_info;
				bmp_param.mode = DRAW_BMP_MODE_VAR_SIZE;
				bmp_param.mode_param.copy_pt = ui_point(get_bmp_icon_width(theme_param->field_skin.field_normal_mid_icon) / 2, get_bmp_icon_height(theme_param->field_skin.field_normal_mid_icon) / 2);
				mid_icon_rect = ui_rect(header_rect.size.width + left_icon_rect.size.width,0,field_rect.size.width - left_icon_rect.size.width - right_icon_rect.size.width,bmp_info.pixel_buf.size.height);
				draw_bitmap(painter, mid_icon_rect, &bmp_param);
			}
		}

	}

	/*field inset*/
	if(theme_param->field_skin.inset_type == EDIT_FIELD_INSET_TYPE_BMP)
	{
		/*inset bmp*/
		gui_memset(&left_inset_icon_rect,0,sizeof(left_inset_icon_rect));
		gui_memset(&mid_inset_icon_rect,0,sizeof(mid_inset_icon_rect));
		gui_memset(&right_inset_icon_rect,0,sizeof(right_inset_icon_rect));
		if(edit_data->high_light)
		{
			/*var mode*/
			if(is_draw_field_inset_light_icon_VAR(&theme_param->field_skin.edit_field_inset.inset_skin))
			{

				bmp_param.mode = DRAW_BMP_MODE_VAR_SIZE;
				gui_get_bmp(theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_left_icon, &bmp_info);
				bmp_param.bmp_info = bmp_info;
				bmp_param.mode_param.copy_pt = ui_point(get_bmp_icon_width(theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_left_icon) / 2, get_bmp_icon_height(theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_left_icon) / 2);
				left_inset_icon_rect  = ui_rect(0,0,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
				left_inset_icon_rect = rect_align(left_inset_icon_rect.size, field_rect, UI_ALIGN_CENTER_VER);
				left_inset_icon_rect = ui_rect(edit_data->header_width + theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_left_icon_margin.left, left_inset_icon_rect.point.y, \
						field_rect.size.width - theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_left_icon_margin.left - theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_left_icon_margin.right, \
						bmp_info.pixel_buf.size.height);
				draw_bitmap(painter, left_inset_icon_rect, &bmp_param);
			}/*normal mode*/
			else
			{
				/*inset left*/
				if(theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_left_icon)
				{
					bmp_param.mode = DRAW_BMP_MODE_NORMAL;
					gui_get_bmp(theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_left_icon,&bmp_info);
					left_inset_icon_rect  = ui_rect(0,0,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
					left_inset_icon_rect = rect_align(left_inset_icon_rect.size, field_rect, UI_ALIGN_CENTER_VER);
					left_inset_icon_rect = ui_rect(edit_data->header_width + theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_left_icon_margin.left,left_inset_icon_rect.point.y,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
					bmp_param.bmp_info = bmp_info;
					draw_bitmap(painter, left_inset_icon_rect, &bmp_param);
				}
				/*inset right*/
				if(theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_righ_icon)
				{
					bmp_param.mode = DRAW_BMP_MODE_NORMAL;
					gui_get_bmp(theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_righ_icon,&bmp_info);
					right_inset_icon_rect  = ui_rect(0,0,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
					right_inset_icon_rect = rect_align(right_inset_icon_rect.size, field_rect, UI_ALIGN_CENTER_VER);
					right_inset_icon_rect = ui_rect(rect.size.width - theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_righ_icon_margin.right - right_inset_icon_rect.size.width,\
							right_inset_icon_rect.point.y,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
					bmp_param.bmp_info = bmp_info;
					draw_bitmap(painter, right_inset_icon_rect, &bmp_param);
				}

				left_inset_width = theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_left_icon_margin.left \
						+ left_inset_icon_rect.size.width + theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_left_icon_margin.right;
				right_inset_width = theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_righ_icon_margin.left \
						+ right_inset_icon_rect.size.width + theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_righ_icon_margin.right;
				/*inset mid*/
				if(theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_mid_icon)
				{
					gui_get_bmp(theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_mid_icon,&bmp_info);
					bmp_param.bmp_info = bmp_info;
					bmp_param.mode = DRAW_BMP_MODE_VAR_SIZE;
					bmp_param.mode_param.copy_pt = ui_point(get_bmp_icon_width(theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_mid_icon) / 2, get_bmp_icon_height(theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_mid_icon) / 2);
					mid_inset_icon_rect  = ui_rect(0,0,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
					mid_inset_icon_rect = rect_align(mid_inset_icon_rect.size, field_rect, UI_ALIGN_CENTER_VER);
					mid_inset_icon_rect = ui_rect(edit_data->header_width + left_inset_width, mid_inset_icon_rect.point.y, field_rect.size.width - left_inset_width - right_inset_width, bmp_info.pixel_buf.size.height);
					draw_bitmap(painter, mid_inset_icon_rect, &bmp_param);
				}
				/*draw inset right again*/
				if(theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_mid_icon && theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_left_icon)
				{/*inset right*/
					bmp_param.mode = DRAW_BMP_MODE_NORMAL;
					gui_get_bmp(theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_left_icon,&bmp_info);
					right_inset_icon_rect  = ui_rect(0,0,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
					right_inset_icon_rect = rect_align(right_inset_icon_rect.size, field_rect, UI_ALIGN_CENTER_VER);
					right_inset_icon_rect = ui_rect(rect.size.width - theme_param->field_skin.edit_field_inset.inset_skin.field_light_inset_righ_icon_margin.right - right_inset_icon_rect.size.width,\
							right_inset_icon_rect.point.y,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
					bmp_param.bmp_info = bmp_info;
					draw_bitmap(painter, right_inset_icon_rect, &bmp_param);
				}
			}
		}
		else
		{
			/*var mode*/
			if(is_draw_field_inset_normal_icon_VAR(&theme_param->field_skin.edit_field_inset.inset_skin))
			{
				bmp_param.mode = DRAW_BMP_MODE_VAR_SIZE;
				gui_get_bmp(theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_left_icon, &bmp_info);
				bmp_param.bmp_info = bmp_info;
				bmp_param.mode_param.copy_pt = ui_point(get_bmp_icon_width(theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_left_icon) / 2, get_bmp_icon_height(theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_left_icon) / 2);
				left_inset_icon_rect  = ui_rect(0,0,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
				left_inset_icon_rect = rect_align(left_inset_icon_rect.size, field_rect, UI_ALIGN_CENTER_VER);
				left_inset_icon_rect = ui_rect(edit_data->header_width + theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_left_icon_margin.left, left_inset_icon_rect.point.y, \
						field_rect.size.width - theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_left_icon_margin.left - theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_left_icon_margin.right, \
						bmp_info.pixel_buf.size.height);
				draw_bitmap(painter, left_inset_icon_rect, &bmp_param);
			}/*normal mode*/
			else
			{
				/*inset left*/
				if(theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_left_icon)
				{
					bmp_param.mode = DRAW_BMP_MODE_NORMAL;
					gui_get_bmp(theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_left_icon,&bmp_info);
					left_inset_icon_rect  = ui_rect(0,0,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
					left_inset_icon_rect = rect_align(left_inset_icon_rect.size, field_rect, UI_ALIGN_CENTER_VER);
					left_inset_icon_rect = ui_rect(edit_data->header_width + theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_left_icon_margin.left,left_inset_icon_rect.point.y,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
					bmp_param.bmp_info = bmp_info;
					draw_bitmap(painter, left_inset_icon_rect, &bmp_param);
				}
				/*inset right*/
				if(theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_righ_icon)
				{
					bmp_param.mode = DRAW_BMP_MODE_NORMAL;
					gui_get_bmp(theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_righ_icon,&bmp_info);
					right_inset_icon_rect  = ui_rect(0,0,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
					right_inset_icon_rect = rect_align(right_inset_icon_rect.size, field_rect, UI_ALIGN_CENTER_VER);
					right_inset_icon_rect = ui_rect(rect.size.width - theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_righ_icon_margin.right - right_inset_icon_rect.size.width,\
							right_inset_icon_rect.point.y,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
					bmp_param.bmp_info = bmp_info;
					draw_bitmap(painter, right_inset_icon_rect, &bmp_param);
				}

				left_inset_width = theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_left_icon_margin.left \
						+ left_inset_icon_rect.size.width + theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_left_icon_margin.right;
				right_inset_width = theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_righ_icon_margin.left \
						+ right_inset_icon_rect.size.width + theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_righ_icon_margin.right;

				/*inset mid*/
				if(theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_mid_icon)
				{
					gui_get_bmp(theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_mid_icon,&bmp_info);
					bmp_param.bmp_info = bmp_info;
					bmp_param.mode = DRAW_BMP_MODE_VAR_SIZE;
					bmp_param.mode_param.copy_pt = ui_point(get_bmp_icon_width(theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_mid_icon) / 2, get_bmp_icon_height(theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_mid_icon) / 2);
					mid_inset_icon_rect  = ui_rect(0,0,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
					mid_inset_icon_rect = rect_align(mid_inset_icon_rect.size, field_rect, UI_ALIGN_CENTER_VER);
					mid_inset_icon_rect = ui_rect(edit_data->header_width + left_inset_width, mid_inset_icon_rect.point.y, field_rect.size.width - left_inset_width - right_inset_width, bmp_info.pixel_buf.size.height);
					draw_bitmap(painter, mid_inset_icon_rect, &bmp_param);
				}
				/*draw inset right again*/
				if(theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_mid_icon && theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_left_icon)
				{/*inset right*/
					bmp_param.mode = DRAW_BMP_MODE_NORMAL;
					gui_get_bmp(theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_righ_icon,&bmp_info);
					right_inset_icon_rect  = ui_rect(0,0,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
					right_inset_icon_rect = rect_align(right_inset_icon_rect.size, field_rect, UI_ALIGN_CENTER_VER);
					right_inset_icon_rect = ui_rect(rect.size.width - theme_param->field_skin.edit_field_inset.inset_skin.field_normal_inset_righ_icon_margin.right - right_inset_icon_rect.size.width,\
							right_inset_icon_rect.point.y,bmp_info.pixel_buf.size.width,bmp_info.pixel_buf.size.height);
					bmp_param.bmp_info = bmp_info;
					draw_bitmap(painter, right_inset_icon_rect, &bmp_param);
				}
			}
		}

	}
	else if(theme_param->field_skin.inset_type == EDIT_FIELD_INSET_TYPE_DRAW)
	{
		if(theme_param->field_skin.edit_field_inset.inset_graphic_draw.inset_graphic_on_draw)
		{
			theme_param->field_skin.edit_field_inset.inset_graphic_draw.inset_graphic_on_draw(self,(void *)theme_param);
		}

		if(edit_data->high_light)
		{
			left_inset_width = theme_param->field_skin.edit_field_inset.inset_graphic_draw.inset_light_graphic_width \
					+ theme_param->field_skin.edit_field_inset.inset_graphic_draw.field_light_inset_left_graphic_margin.left \
					+ theme_param->field_skin.edit_field_inset.inset_graphic_draw.field_light_inset_left_graphic_margin.right;
			right_inset_width = theme_param->field_skin.edit_field_inset.inset_graphic_draw.inset_light_graphic_width \
					+ theme_param->field_skin.edit_field_inset.inset_graphic_draw.field_light_inset_righ_graphic_margin.left \
					+ theme_param->field_skin.edit_field_inset.inset_graphic_draw.field_light_inset_righ_graphic_margin.right;
		}
		else
		{
			left_inset_width = theme_param->field_skin.edit_field_inset.inset_graphic_draw.inset_normal_graphic_width \
					+ theme_param->field_skin.edit_field_inset.inset_graphic_draw.field_normal_inset_left_graphic_margin.left \
					+ theme_param->field_skin.edit_field_inset.inset_graphic_draw.field_normal_inset_left_graphic_margin.right;
			right_inset_width = theme_param->field_skin.edit_field_inset.inset_graphic_draw.inset_normal_graphic_width \
					+ theme_param->field_skin.edit_field_inset.inset_graphic_draw.field_normal_inset_righ_graphic_margin.left \
					+ theme_param->field_skin.edit_field_inset.inset_graphic_draw.field_normal_inset_righ_graphic_margin.right;
		}
	}

	/*draw field text*/
	if(edit_data->high_light)
	{
		text_param.color = theme_param->edit_theme_rect.field_light_color;
	}
	else
	{
		text_param.color = theme_param->edit_theme_rect.field_normal_color;
	}

	text_rect.size.width = get_text_width(field_data, -1, text_param.font_size, -1);
	text_rect.size.height = get_text_height(text_param.font_size, -1);

	if(theme_param->edit_theme_rect.field_centered)
	{
		text_rect = rect_align(text_rect.size, field_rect, UI_ALIGN_CENTER);
	}
	else
	{
		text_rect = rect_align(text_rect.size, field_rect, UI_ALIGN_CENTER_VER);
		if(theme_param->edit_theme_rect.field_margin.left < MAX(left_inset_width, left_icon_rect.size.width))
		{
			theme_param->edit_theme_rect.field_margin.left = MAX(left_inset_width, left_icon_rect.size.width);
		}

		if(theme_param->edit_theme_rect.field_margin.left + text_rect.size.width + MAX(right_inset_width, right_icon_rect.size.width) > field_rect.size.width)
		{
			theme_param->edit_theme_rect.field_margin.left = field_rect.size.width - text_rect.size.width - MAX(right_inset_width, right_icon_rect.size.width);
		}
		text_rect.point.x = field_rect.point.x + theme_param->edit_theme_rect.field_margin.left;
	}
	draw_text(painter,text_rect,field_data, -1, &text_param);
	return TRUE;
}


static BOOL edit_item_on_draw(UIEditItem *self)
{
	UIEditItemThemeDefault default_theme;
	UIEditItemData *edit_data = (UIEditItemData *)self->private_data;
	gui_memset(&default_theme, 0, sizeof(UIEditItemThemeDefault));
	default_theme.edit_default_rect.header_bkg_light_color = 0xff38e7d9;
	default_theme.edit_default_rect.header_bkg_normal_color = 0xff000000;
	default_theme.edit_default_rect.field_bkg_light_color = 0xff38e7d9;
	default_theme.edit_default_rect.field_bkg_normal_color = 0xff000000;
	default_theme.edit_default_rect.header_normal_color = 0x9cb9e1;
	default_theme.edit_default_rect.header_light_color = WHITE_COLOR;
	default_theme.edit_default_rect.field_light_color = WHITE_COLOR;
	default_theme.edit_default_rect.field_normal_color = 0x9cb9e1;
	default_theme.edit_default_rect.font_size = -1;
	default_theme.edit_default_rect.header_margin.left = 50;
	default_theme.edit_default_rect.field_margin.left = 100;
	edit_data->edit_field.select_field.select_action = edit_item_popup_metal_customer_action;

	return edit_item_theme_rect(self, &default_theme);
}


static BOOL edit_item_on_destroy(UIEditItem *self)
{
	delete_edit_item_theme_data(self);
	delete_edit_item_private_data((UIEditItemData *)self->private_data);
	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UIEditItem* create_edit_item(const char *id)
{
	UIWindow *edit_item = NULL;

	register_edit_item_class();
	edit_item = create_control(id, 0, 0, EDIT_ITEM_DEFAULT_WIDTH, EDIT_ITEM_DEFAULT_HEIGHT);
	if (edit_item != NULL)
	{
		UIEditItemData *edit_data;

		edit_data = new_edit_item_private_data();
		if (edit_data == NULL)
		{
			gui_free(edit_item);
			return NULL;
		}

		edit_item->private_data = edit_data;
		edit_item->focus_stop = TRUE;
		edit_item->class_name = EDIT_ITEM_CLASS_NAME;
		edit_item->on_draw = edit_item_on_draw;
		edit_item->on_destroy = edit_item_on_destroy;
	}
	return edit_item;
}


int set_edit_item_header_text(UIEditItem *edit, const char *text)
{
	UIEditItemData *edit_data;
	int ret = 0;
    if (!edit || !text)
	{
		return -1;
	}
    edit_data = (UIEditItemData *)edit->private_data;
    ret = set_edit_item_header_internal(edit_data,text);
    invalidate_window(edit);
    return ret;
}


int set_edit_item_field_text(UIEditItem *edit, const char *text)
{
	UIEditItemData *edit_data;
	int ret = 0;
    if (!edit || !text)
	{
		return -1;
	}
    edit_data = (UIEditItemData *)edit->private_data;
    ret = set_edit_item_field_internal(edit_data,text);
    invalidate_window(edit);
	return ret;
}


int set_edit_item_field_type(UIEditItem *edit, int field_type)
{
	UIEditItemData *edit_data;
    if (!edit)
	{
		return -1;
	}
    edit_data = edit->private_data;
    edit_data->field_type = field_type;
	return 0;
}


int set_edit_item_high_light(UIEditItem *edit, BOOL high_light)
{
	UIEditItemData *edit_data;
    if (!edit)
	{
		return -1;
	}
    edit_data = edit->private_data;

    if(edit_data->high_light != high_light)
    {
        edit_data->high_light = high_light;
        invalidate_window(edit);
    }
	return 0;
}


int set_edit_item_header_width(UIEditItem *edit, U32 header_width)
{
	UIEditItemData *edit_data;
    if (!edit)
	{
		return -1;
	}
    edit_data = edit->private_data;
    edit_data->header_width = header_width;
    invalidate_window(edit);
	return 0;
}


UIRect * calc_adapt_popup_rect(UIWindow *parent, UIEditItem *edit, UIRect *popu_rect)
{
	if(!parent || !edit || !popu_rect)
	{
		return NULL;
	}
	int edit_center_y;
	/*H*/
	if(popu_rect->size.width > parent->rect.size.width - (popu_rect->point.x + 1))
	{
		popu_rect->point.x -= popu_rect->size.width - (parent->rect.size.width - (popu_rect->point.x + 1));
		if(popu_rect->point.x < 0)
		{
			popu_rect->point.x = 0;
		}
	}
	/*V*/
	if(popu_rect->size.height > parent->rect.size.height - (popu_rect->point.y + 1))
	{
		edit_center_y = edit->rect.point.y + edit->rect.size.height/2;
		//below the edit center y
		if(popu_rect->point.y > edit_center_y)
		{
			if(popu_rect->size.height < popu_rect->point.y - 2*(popu_rect->point.y - edit_center_y) + 1)
			{
				popu_rect->point.y = popu_rect->point.y - 2*(popu_rect->point.y - edit_center_y) - popu_rect->size.height;
				if(popu_rect->point.y < 0)
				{
					popu_rect->point.y = 0;
				}
			}
			else
			{
				if((edit_center_y + 1) > parent->rect.size.height - (edit_center_y + 1))
				{
					popu_rect->size.height = popu_rect->point.y - 2*(popu_rect->point.y - edit_center_y) + 1;
					popu_rect->point.y = 0;
				}
				else
				{
					popu_rect->size.height = parent->rect.size.height - (popu_rect->point.y + 1);
				}
			}
		}
		else
		{
			if(popu_rect->size.height < popu_rect->point.y + 1)
			{
				popu_rect->point.y = popu_rect->point.y - popu_rect->size.height;
				if(popu_rect->point.y < 0)
				{
					popu_rect->point.y = 0;
				}
			}
			else
			{
				if((edit_center_y + 1) > parent->rect.size.height - (edit_center_y + 1))
				{
					popu_rect->size.height = popu_rect->point.y + 1;
					popu_rect->point.y = 0;
				}
				else
				{
					popu_rect->size.height = parent->rect.size.height - (popu_rect->point.y + 1);
				}
			}
		}
	}
	popu_rect->point = screen_to_client(parent, popu_rect->point);
	return popu_rect;
}


int edit_item_popup_metal_customer_action(UIPage *page, UIEditItem *edit_item)
{
	UIEditItemData *edit_data;
	if (!edit_item || !page)
	{
		return -1;
	}
    edit_data = edit_item->private_data;
	int popu_x = edit_item->rect.point.x + edit_data->header_width + 43;
	int popu_y = edit_item->rect.point.y + edit_item->rect.size.height + 3;
	UIRect popu_rect = ui_rect(popu_x, popu_y, 500, 250);
    UIPopup *popup = create_popup("EditItemPopupPanel");
//    window_use_style(popup, "Edit Item Popup");
    add_sub_window(page, popup);
    if (calc_adapt_popup_rect(page, edit_item, &popu_rect))
	{
    	set_window_rect(popup, popu_rect);
	}
    flush_window(page);
	destroy_window(popup);
	return 0;
}


int edit_item_field_inset_draw_graphic_triangle(UIEditItem *edit, void *param)
{
	printf("\ndraw triangle\n");
	UIEditItemThemeBmp *theme_param = (UIEditItemThemeBmp *)param;
	int light_left_inset_margin_left;
	int light_right_inset_margin_right;
	int normal_left_inset_margin_left;
	int normal_right_inset_margin_right;
	U32 light_color;
	U32 normal_color;
	U16 len;
	U16 cy;
	U16 cx;
	BOOL normal_inset_show;
	UIEditItemData *edit_data;

    if (!edit || !theme_param)
	{
		return -1;
	}
    len = 12;
    normal_inset_show = FALSE;
    edit_data = edit->private_data;
	UIPainter *painter = &(edit->painter);
	UIRect rect = ui_rect(0, 0, edit->rect.size.width, edit->rect.size.height);
	UIRect field_rect = ui_rect(edit_data->header_width, 0, rect.size.width - edit_data->header_width, rect.size.height);
    if(edit_data->high_light)
    {
    	light_left_inset_margin_left = theme_param->field_skin.edit_field_inset.inset_graphic_draw.field_light_inset_left_graphic_margin.left;
    	light_right_inset_margin_right = theme_param->field_skin.edit_field_inset.inset_graphic_draw.field_light_inset_left_graphic_margin.right;
    	light_color = theme_param->field_skin.edit_field_inset.inset_graphic_draw.inset_graphic_light_color;
    	theme_param->field_skin.edit_field_inset.inset_graphic_draw.inset_light_graphic_width = len -1;
    	/*draw arrow left */
    	cy = (rect.size.height - (len-1)*2)/2+len-1;
    	cx = field_rect.point.x + light_left_inset_margin_left + len-1;
    	draw_triangle(painter,cx,cy, 1, len, light_color);
    	/*draw arrow right */
    	cx = field_rect.point.x + field_rect.size.width - (len - 1) - light_right_inset_margin_right;
    	draw_triangle(painter,cx,cy, 0, len, light_color);
    }
    else if(FALSE == edit_data->high_light && normal_inset_show)
    {
    	normal_left_inset_margin_left = theme_param->field_skin.edit_field_inset.inset_graphic_draw.field_normal_inset_left_graphic_margin.left;
    	normal_right_inset_margin_right = theme_param->field_skin.edit_field_inset.inset_graphic_draw.field_normal_inset_left_graphic_margin.right;
    	normal_color = theme_param->field_skin.edit_field_inset.inset_graphic_draw.inset_graphic_normal_color;
    	theme_param->field_skin.edit_field_inset.inset_graphic_draw.inset_normal_graphic_width = len -1;
    	/*draw arrow left */
    	cy = (rect.size.height - (len-1)*2)/2+len-1;
    	cx = field_rect.point.x + normal_left_inset_margin_left + len-1;
    	draw_triangle(painter,cx,cy, 1, len, normal_color);
    	/*draw arrow right */
    	cx = field_rect.point.x + field_rect.size.width - (len - 1) - normal_right_inset_margin_right;
    	draw_triangle(painter,cx,cy, 0, len, normal_color);
    }
    return 0;
}


int edit_item_field_inset_draw_graphic_arrow(UIEditItem *edit, void *param)
{
	UIEditItemThemeBmp *theme_param = (UIEditItemThemeBmp *)param;
    if (!edit || !theme_param)
	{
		return -1;
	}
	int light_left_inset_margin_left;
	int light_right_inset_margin_right;
	int normal_left_inset_margin_left;
	int normal_right_inset_margin_right;
	UIRect text_rect;
	DrawTextParam text_param;
	BOOL normal_inset_show = theme_param->field_skin.edit_field_inset.inset_graphic_draw.normal_inset_show;
	UIEditItemData *edit_data = edit->private_data;
	UIPainter *painter = &(edit->painter);
	UIRect rect = ui_rect(0, 0, edit->rect.size.width, edit->rect.size.height);
	UIRect field_rect = ui_rect(edit_data->header_width, 0, rect.size.width - edit_data->header_width, rect.size.height);
	default_draw_text_param(&text_param);
	text_param.font_size = theme_param->edit_theme_rect.font_size;
    if(edit_data->high_light)
    {
    	text_rect.size.width = get_text_width("<", -1, text_param.font_size, -1);
    	text_rect.size.height = get_text_height(text_param.font_size, -1);
    	light_left_inset_margin_left = theme_param->field_skin.edit_field_inset.inset_graphic_draw.field_light_inset_left_graphic_margin.left;
    	light_right_inset_margin_right = theme_param->field_skin.edit_field_inset.inset_graphic_draw.field_light_inset_righ_graphic_margin.right;
    	text_param.color = theme_param->field_skin.edit_field_inset.inset_graphic_draw.inset_graphic_light_color;
    	theme_param->field_skin.edit_field_inset.inset_graphic_draw.inset_light_graphic_width = text_rect.size.width;
    	/*draw arrow left */
    	text_rect = rect_align(text_rect.size,field_rect, UI_ALIGN_CENTER_VER);
    	text_rect.point.x = field_rect.point.x + light_left_inset_margin_left;
    	draw_text(painter, text_rect, "<", -1, &text_param);
    	/*draw arrow right */
    	text_rect.point.x = rect.size.width - light_right_inset_margin_right - text_rect.size.width;
    	draw_text(painter, text_rect, ">", -1, &text_param);
    }
    else if(FALSE == edit_data->high_light && normal_inset_show)
    {
    	text_rect.size.width = get_text_width("<", -1, text_param.font_size, -1);
    	text_rect.size.height = get_text_height(text_param.font_size, -1);
    	normal_left_inset_margin_left = theme_param->field_skin.edit_field_inset.inset_graphic_draw.field_normal_inset_left_graphic_margin.left;
    	normal_right_inset_margin_right = theme_param->field_skin.edit_field_inset.inset_graphic_draw.field_normal_inset_righ_graphic_margin.right;
    	text_param.color = theme_param->field_skin.edit_field_inset.inset_graphic_draw.inset_graphic_normal_color;
    	theme_param->field_skin.edit_field_inset.inset_graphic_draw.inset_light_graphic_width = text_rect.size.width;
    	/*draw arrow left */
    	text_rect = rect_align(text_rect.size,field_rect, UI_ALIGN_CENTER_VER);
    	text_rect.point.x = field_rect.point.x + normal_left_inset_margin_left;
    	draw_text(painter, text_rect, "<", -1, &text_param);
    	/*draw arrow right */
    	text_rect.point.x = rect.size.width - normal_right_inset_margin_right - text_rect.size.width;
    	draw_text(painter, text_rect, ">", -1, &text_param);
    }
	return 0;
}
