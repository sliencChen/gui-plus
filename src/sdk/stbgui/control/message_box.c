/*
 * message_box.c
 *
 *  Created on: 2019-7-
 *      Author: Silence
 */

#include <gui_window.h>
#include <control/message_box.h>


//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------
static char *yes_or_no_option[3] = {"No", "Yes","Cancel"};
static char password[6] = {'1','2','3','4','5','6'};


//-------------------------------------------------------------------------------------------------
// internal use
//-------------------------------------------------------------------------------------------------
void default_message_box_data(UIMessageBoxData *message_box_data)
{
	default_popup_data(&(message_box_data->popup));

	init_string(&(message_box_data->content));
	message_box_data->msg_box_action_type = 0;
	message_box_data->text_field = NULL;
	message_box_data->msg_box_sizer = NULL;
	message_box_data->duration = 0;

	message_box_data->select.option_num = 3;
	message_box_data->select.option_string = yes_or_no_option;
	message_box_data->select.selected_option = 0;
}


static UIMessageBoxData *new_message_box_data(void)
{
	UIMessageBoxData *message_box_data;

	message_box_data = (UIMessageBoxData *)gui_malloc(sizeof(UIMessageBoxData));
	if(message_box_data == NULL)
	{
		return NULL;
	}
	default_message_box_data(message_box_data);
	message_box_data->text_field = create_text_field("MessageBoxText");
	message_box_data->msg_box_sizer = create_box_sizer(UI_SIZER_DIR_VER);
	return message_box_data;
}


static void delete_message_box_private_data(UIMessageBoxData *message_box_data)
{
	delete_popup_data(&(message_box_data->popup));
	destroy_string(&(message_box_data->content));
}


static int set_message_box_string_internal(UIString *string, const char *text)
{
	return set_string_text(string, text);
}


//-------------------------------------------------------------------------------------------------
// window manager callback
//-------------------------------------------------------------------------------------------------
static BOOL message_box_on_draw(UIMessageBox *self)
{
	UIMessageBoxThemeDefaultData  default_message_box;
	UIMessageBoxData *message_box_data = (UIMessageBoxData *)self->private_data;
	gui_memset(&default_message_box, 0 ,sizeof(UIMessageBoxThemeDefaultData));
	/*popup*/
	default_message_box.popup_theme_default_data.title_color = RED_COLOR;
	default_message_box.popup_theme_default_data.title_bkg_color = 0xFF9BCD9B;
	default_message_box.popup_theme_default_data.border_color = 0xFF9932CC;
	default_message_box.popup_theme_default_data.inside_bkg_color = 0xFF98F5FF;
	default_message_box.popup_theme_default_data.border_size = 2;
	default_message_box.popup_theme_default_data.title_height = 60;
	default_message_box.popup_theme_default_data.font_size = -1;

	if (SELECT_OPTION_TYPE == message_box_data->msg_box_action_type)
	{
		/*select action type*/
		default_message_box.select_field_default_data.option_item.item_text_color = BLACK_COLOR;
		default_message_box.select_field_default_data.visible_option_num = 1;
		default_message_box.select_field_default_data.option_item_space = 50;
		default_message_box.select_field_default_data.option_item.item_inset_high_light_color = RED_COLOR;
		default_message_box.select_field_default_data.option_item.item_size = ui_size(160,40);
		default_message_box.select_field_default_data.option_item.item_inset_normal_color = BLUE_COLOR;
		default_message_box.select_field_default_data.option_item.item_border_size = 0;
		default_message_box.select_field_default_data.option_item.item_border_color = 0xFF98F5FF;

		default_message_box.select_field_default_data.option_item.item_text_color = WHITE_COLOR;
		default_message_box.select_field_default_data.option_item.font_size = -1;

		default_message_box.select_field_default_data.inset_graphic_color = BLACK_COLOR;
		default_message_box.select_field_default_data.field_inset_left_graphic_margin.left = 10;
		default_message_box.select_field_default_data.field_inset_righ_graphic_margin.right =10;
		default_message_box.select_field_default_data.inset_graphic_on_draw = message_box_select_field_inset_draw_graphic_default;
	}
	else if (INPUT_NUMERIC_TYPE == message_box_data->msg_box_action_type)
	{
		/*input action type*/
		default_message_box.input_field_default_data.input_item_space = 10;
		default_message_box.input_field_default_data.input_item.border_color = 0xFF9932CC;
		default_message_box.input_field_default_data.input_item.font_size = -1;
		default_message_box.input_field_default_data.input_item.item_bkg_color = WHITE_COLOR;
		default_message_box.input_field_default_data.input_item.item_size = ui_size(40, 40);
		default_message_box.input_field_default_data.input_item.text_high_light_color = RED_COLOR;
		default_message_box.input_field_default_data.input_item.text_normal_color = BLACK_COLOR;
		default_message_box.input_field_default_data.numeric_is_show = FALSE;
	}

	set_text_field_text_color(message_box_data->text_field, BLACK_COLOR);
	return message_box_theme_default(self, &default_message_box);
}


static BOOL message_box_on_destroy(UIMessageBox *self)
{
	delete_message_box_private_data(self->private_data);
	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// class event
//-------------------------------------------------------------------------------------------------
static int message_box_on_left_key(struct _ui_window_ *window, Event event, EventData event_data)
{
	UIMessageBox *message_box = window;
	UIMessageBoxData *message_box_data = message_box->private_data;
	int select = 0;
	select = get_message_box_selected_option(message_box);
	if (-1 != select)
	{
		select--;
	}

	if (select < 0)
	{
		select = message_box_data->select.option_num - 1;
	}
	set_message_box_selected_option(message_box, select);
	return 0;
}


static int message_box_on_right_key(struct _ui_window_ *window, Event event, EventData event_data)
{
	UIMessageBox *message_box = window;
	UIMessageBoxData *message_box_data = message_box->private_data;
	int select = 0;
	select = get_message_box_selected_option(message_box);
	if (-1 != select)
	{
		select++;
	}

	if (select >= message_box_data->select.option_num)
	{
		select = 0;
	}
	set_message_box_selected_option(message_box, select);
	return 0;
}


BOOL register_message_box_event(void)
{
	BOOL result = TRUE;

	result &= register_class_event_handler(MESSAGE_BOX_CLASS_NAME, GUI_EVT_VK_LEFT, message_box_on_left_key);
	result &= register_class_event_handler(MESSAGE_BOX_CLASS_NAME, GUI_EVT_VK_RIGHT, message_box_on_right_key);
	result &= register_class_event_handler(MESSAGE_BOX_CLASS_NAME, GUI_EVT_VK_SELECT, 0);
	return result;
}


static BOOL register_message_box_class(void)
{
	static BOOL registered = FALSE;
	BOOL res;

	if (registered) return FALSE;

	res = register_message_box_event();
	registered = TRUE;

	return res;
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UIMessageBox *create_message_box(const char *id)
{
	UIMessageBox *message_box;
	register_message_box_class();
	message_box = create_sub_window(id, 0, 0, MESSAGE_BOX_DEFAULT_WIDTH, MESSAGE_BOX_DEFAULT_HEIGHT);
	if(message_box)
	{
		UIMessageBoxData *message_box_data = new_message_box_data();
		if(message_box_data == NULL)
		{
			destroy_window(message_box);
			return NULL;
		}

		message_box->private_data = message_box_data;
		message_box->focus_stop = TRUE;
		message_box->class_name = MESSAGE_BOX_CLASS_NAME;
		message_box->on_draw = message_box_on_draw;
		message_box->on_destroy = message_box_on_destroy;

        if (message_box_data->text_field)
        {
        	add_sub_window(message_box, message_box_data->text_field);
        	if (message_box_data->msg_box_sizer)
        	{
            	sizer_add_window(message_box_data->msg_box_sizer, message_box_data->text_field, ui_size(200, 40), 0, 0, UI_ALIGN_CENTER_VER);
        	}
        }
	}
	return message_box;
}


int set_message_box_title(UIMessageBox *message_box, const char *text)
{
	UIMessageBoxData *message_box_data;
	if (!message_box || !message_box->private_data || !text)
	{
		return -1;
	}

	message_box_data = (UIMessageBoxData *)message_box->private_data;
	set_message_box_string_internal(&message_box_data->popup.title, text);
	invalidate_window(message_box);
	return 0;
}


int set_message_box_content(UIMessageBox *message_box, const char *text)
{
	UIMessageBoxData *message_box_data;
	int line_height;
	int line_num;
	UISize mini_size;
	UISize text_size;
	if (!message_box || !message_box->private_data || !text)
	{
		return -1;
	}

	message_box_data = (UIMessageBoxData *)message_box->private_data;
	set_message_box_string_internal(&message_box_data->content, text);
	if (message_box_data->text_field)
	{
		/*adjust the text field (now just can adjust the height)*/
		text_size = ui_size(message_box->rect.size.width, message_box->rect.size.height - 80);
		set_window_rect(message_box_data->text_field, ui_rect(0, 0, text_size.width, text_size.height));
		set_text_field_text(message_box_data->text_field, message_box_data->content.content, 0);
		line_num = get_text_field_line_num(message_box_data->text_field);
		line_height = get_text_field_line_height(message_box_data->text_field);
		mini_size = ui_size(text_size.width, line_num*line_height);
		message_box_data->text_field->min_size = mini_size;
		set_text_field_align_center(message_box_data->text_field, TRUE);
		set_container_inset(message_box, ui_inset(20, 80, 20, 100));
		if (message_box_data->msg_box_sizer)
		{
			set_message_box_sizer(message_box, message_box_data->msg_box_sizer);
		}
	}
	invalidate_window(message_box);
	return 0;
}


int	set_message_box_druation(UIMessageBox *message_box, int duration_ms)
{
	UIMessageBoxData *message_box_data;
	if (!message_box || !message_box->private_data)
	{
		return -1;
	}

	message_box_data = (UIMessageBoxData *)message_box->private_data;
	message_box_data->duration = duration_ms;
	invalidate_window(message_box);
	return 0;
}


int	set_message_action_type(UIMessageBox *message_box, int msg_box_action_type)
{
	UIMessageBoxData *message_box_data;
	if (!message_box || !message_box->private_data)
	{
		return -1;
	}

	message_box_data = (UIMessageBoxData *)message_box->private_data;
	message_box_data->msg_box_action_type = msg_box_action_type;
	if (SELECT_OPTION_TYPE == msg_box_action_type)
	{
		message_box_data->select.option_string = yes_or_no_option;
	}
	else if (INPUT_NUMERIC_TYPE == msg_box_action_type)
	{
		message_box_data->input.input_numeric = password;
		message_box_data->input.input_position = 0;
	}
	return 0;
}


int set_message_box_option_string_array(UIMessageBox *message_box, const char **option_string_array)
{
	UIMessageBoxData *message_box_data;
	if (!message_box || !message_box->private_data || !option_string_array)
	{
		return -1;
	}

	message_box_data = (UIMessageBoxData *)message_box->private_data;
	if (message_box_data->msg_box_action_type != SELECT_OPTION_TYPE)
	{
		return -1;
	}
	message_box_data->select.option_string = option_string_array;
	invalidate_window(message_box);
	return 0;
}


int set_message_box_option_num(UIMessageBox *message_box, int option_num)
{

	UIMessageBoxData *message_box_data;
	if (!message_box || !message_box->private_data || option_num <= 0)
	{
		return -1;
	}

	message_box_data = (UIMessageBoxData *)message_box->private_data;
	if (message_box_data->msg_box_action_type != SELECT_OPTION_TYPE)
	{
		return -1;
	}
	message_box_data->select.option_num = option_num;
	invalidate_window(message_box);
	return 0;
}


int get_message_box_option_num(UIMessageBox *message_box)
{
	UIMessageBoxData *message_box_data;
	if (!message_box || !message_box->private_data)
	{
		return -1;
	}

	message_box_data = (UIMessageBoxData *)message_box->private_data;
	if (message_box_data->msg_box_action_type != SELECT_OPTION_TYPE)
	{
		return -1;
	}

	return message_box_data->select.option_num;
}


int set_message_box_selected_option(UIMessageBox *message_box, int selected_option)
{
	UIMessageBoxData *message_box_data;
	if (!message_box || !message_box->private_data)
	{
		return -1;
	}

	message_box_data = (UIMessageBoxData *)message_box->private_data;
	if (message_box_data->msg_box_action_type != SELECT_OPTION_TYPE)
	{
		return -1;
	}

	if (selected_option < 0 || selected_option >= message_box_data->select.option_num)
	{
		return -1;
	}
	message_box_data->select.selected_option = selected_option;
	invalidate_window(message_box);
	return 0;
}


int get_message_box_selected_option(UIMessageBox *message_box)
{
	UIMessageBoxData *message_box_data;
	if (!message_box || !message_box->private_data)
	{
		return -1;
	}

	message_box_data = (UIMessageBoxData *)message_box->private_data;
	if (message_box_data->msg_box_action_type != SELECT_OPTION_TYPE)
	{
		return -1;
	}

	return message_box_data->select.selected_option;
}


char *get_message_box_selected_option_string(UIMessageBox *message_box)
{
	UIMessageBoxData *message_box_data;
	char *selected_option_string = NULL;
	if (!message_box || !message_box->private_data)
	{
		return NULL;
	}

	message_box_data = (UIMessageBoxData *)message_box->private_data;
	if (message_box_data->msg_box_action_type != SELECT_OPTION_TYPE)
	{
		return NULL;
	}


	if (message_box_data->select.selected_option < 0 || message_box_data->select.selected_option >= message_box_data->select.option_num)
	{
		return NULL;
	}

	if (message_box_data->select.option_string)
	{
		if (message_box_data->select.option_string[message_box_data->select.selected_option])
		{
			selected_option_string =  message_box_data->select.option_string[message_box_data->select.selected_option];
		}
	}
	return selected_option_string;
}


int set_message_box_digit_num(UIMessageBox *message_box, int digit_num)
{
	UIMessageBoxData *message_box_data;
	if (!message_box || !message_box->private_data)
	{
		return -1;
	}

	message_box_data = (UIMessageBoxData *)message_box->private_data;
	if (message_box_data->msg_box_action_type != INPUT_NUMERIC_TYPE)
	{
		return -1;
	}
	message_box_data->input.digit_num = digit_num;

	return 0;
}


int get_message_box_digit_num(UIMessageBox *message_box)
{
	UIMessageBoxData *message_box_data;
	if (!message_box || !message_box->private_data)
	{
		return -1;
	}

	message_box_data = (UIMessageBoxData *)message_box->private_data;
	if (message_box_data->msg_box_action_type != INPUT_NUMERIC_TYPE)
	{
		return -1;
	}

	return message_box_data->input.digit_num;
}


int set_message_box_input_numeric(UIMessageBox *message_box, const char *input_numeric)
{
	UIMessageBoxData *message_box_data;
	if (!message_box || !message_box->private_data)
	{
		return -1;
	}

	message_box_data = (UIMessageBoxData *)message_box->private_data;
	if (message_box_data->msg_box_action_type != INPUT_NUMERIC_TYPE)
	{
		return -1;
	}

	message_box_data->input.input_numeric = input_numeric;

	return 0;
}


char *get_message_box_input_numeric(UIMessageBox *message_box)
{
	UIMessageBoxData *message_box_data;
	if (!message_box || !message_box->private_data)
	{
		return NULL;
	}

	message_box_data = (UIMessageBoxData *)message_box->private_data;
	if (message_box_data->msg_box_action_type != INPUT_NUMERIC_TYPE)
	{
		return NULL;
	}

	return message_box_data->input.input_numeric;
}


int set_message_box_input_position(UIMessageBox *message_box, int input_position)
{
	UIMessageBoxData *message_box_data;
	if (!message_box || !message_box->private_data)
	{
		return -1;
	}

	message_box_data = (UIMessageBoxData *)message_box->private_data;
	if (message_box_data->msg_box_action_type != INPUT_NUMERIC_TYPE)
	{
		return -1;
	}
	message_box_data->input.input_position = input_position;

	return 0;
}


int set_message_box_sizer(UIMessageBox *message_box, UISizer *sizer)
{
	UIMessageBoxData *message_box_data;
	UIRect message_box_rect;
	if (!message_box || !message_box->private_data)
	{
		return NULL;
	}

	message_box_data = (UIMessageBoxData *)message_box->private_data;
	set_container_sizer(message_box, sizer);
	if (message_box_data->popup.centered)
	{
		message_box_rect = rect_align(message_box->rect.size, ui_rect(0, 0, main_canvas_width(), main_canvas_height()), UI_ALIGN_CENTER);
		message_box_rect.point = screen_to_client(message_box->parent, message_box_rect.point);
		set_window_rect(message_box, message_box_rect);
	}
	return 0;
}


int set_message_box_centered(UIMessageBox *message_box, BOOL centered)
{
	return set_popup_centered(message_box, centered);
}


//-------------------------------------------------------------------------------------------------
// typical theme
//-------------------------------------------------------------------------------------------------
static UISize calculate_text_size(const char *text, int font_size)
{
	UISize size;

	size.width = get_text_width(text, -1, font_size, 100);
	size.height = get_text_height(font_size, 100);

	return size;
}


int message_box_select_field_inset_draw_graphic_default(UIMessageBox* message_box, void *theme_param, UIRect rect)
{
	UIMessageBoxThemeDefaultData *default_param = (UIMessageBoxThemeDefaultData *)theme_param;
	UIRect text_rect;
	DrawTextParam text_param;
	UIPainter *painter = &(message_box->painter);
	text_rect.size = calculate_text_size("<<", default_param->select_field_default_data.option_item.font_size);

	text_rect = rect_align(text_rect.size, rect, UI_ALIGN_CENTER_VER);
	default_draw_text_param(&text_param);
	text_param.font_size = default_param->select_field_default_data.option_item.font_size;
	text_param.color = default_param->select_field_default_data.inset_graphic_color;
	if (default_param->select_field_default_data.field_inset_left_graphic_margin.left > rect.size.width)
	{
		default_param->select_field_default_data.field_inset_left_graphic_margin.left = rect.size.width - text_rect.size.width;
	}
	text_rect.point.x += default_param->select_field_default_data.field_inset_left_graphic_margin.left;
	draw_text(painter, text_rect, "<<", -1, &text_param);


	text_rect = rect_align(text_rect.size, rect, UI_ALIGN_CENTER_VER);
	default_draw_text_param(&text_param);
	text_param.font_size = default_param->select_field_default_data.option_item.font_size;
	text_param.color = default_param->select_field_default_data.inset_graphic_color;

	if (default_param->select_field_default_data.field_inset_righ_graphic_margin.right > rect.size.width)
	{
		default_param->select_field_default_data.field_inset_righ_graphic_margin.right = rect.size.width - text_rect.size.width;
	}
	text_rect.point.x += rect.size.width - text_rect.size.width - default_param->select_field_default_data.field_inset_righ_graphic_margin.right;
	draw_text(painter, text_rect, ">>", -1, &text_param);
	return 0;
}


BOOL message_box_theme_default(UIMessageBox* message_box, UIMessageBoxThemeDefaultData *theme_param)
{
	UIMessageBoxData *message_box_data = message_box->private_data;
	UIPainter *painter = &(message_box->painter);
	UIRect text_rect ;
	DrawTextParam param;
	UISize text_size = {0, 0};
	int visible_option_num;
   	char input_text[2] = {0};
	int index;
	UIRect action_rect = EMPTY_RECT;
	UIRect action_inset_rect= EMPTY_RECT;

	text_size = calculate_text_size(message_box_data->popup.title.content, theme_param->popup_theme_default_data.font_size);
	//draw title area
    draw_rect_frame_gui(painter, ui_rect(0, 0, message_box->rect.size.width, theme_param->popup_theme_default_data.title_height), \
                        theme_param->popup_theme_default_data.border_size, theme_param->popup_theme_default_data.border_color, \
						theme_param->popup_theme_default_data.title_bkg_color);
	//draw title
    default_draw_text_param(&param);
    param.color = theme_param->popup_theme_default_data.title_color;
	param.font_size = theme_param->popup_theme_default_data.font_size;

	if(message_box->rect.size.width > text_size.width)
    {
        text_rect.point.x = (message_box->rect.size.width - text_size.width)/2;
        text_rect.size.width = text_size.width;
    }
    else
    {
        text_rect.point.x = 5;
        text_rect.size.width = message_box->rect.size.width - 10;
    }

    if( theme_param->popup_theme_default_data.title_height > (text_size.height+4) )
    {
        text_rect.point.y = (theme_param->popup_theme_default_data.title_height - text_size.height)/2 ;
        text_rect.size.height = text_size.height;
    }
    else
    {
        text_rect.point.y = 2;
        text_rect.size.height = message_box->rect.size.height - 4;
    }

    draw_text(painter, text_rect, message_box_data->popup.title.content, -1, &param);

    if(message_box_data->popup.container.inset.top < theme_param->popup_theme_default_data.title_height)
    {
    	message_box_data->popup.container.inset.top =  theme_param->popup_theme_default_data.title_height + 10;
    }

    draw_rect_frame_gui(painter, ui_rect(0, theme_param->popup_theme_default_data.title_height - theme_param->popup_theme_default_data.border_size, \
    					message_box->rect.size.width, message_box->rect.size.height - theme_param->popup_theme_default_data.title_height), \
                        theme_param->popup_theme_default_data.border_size, theme_param->popup_theme_default_data.border_color, \
						theme_param->popup_theme_default_data.inside_bkg_color);

    /*draw message box action field*/
	action_rect = ui_rect(message_box->rect.point.x, message_box_data->text_field->rect.point.y + 40, message_box->rect.size.width, \
			message_box->rect.point.y + message_box->rect.size.height - message_box_data->text_field->rect.point.y - 40);
    if (message_box_data->msg_box_action_type == SELECT_OPTION_TYPE)
    {
    	visible_option_num = theme_param->select_field_default_data.visible_option_num;
    	visible_option_num = visible_option_num < message_box_data->select.option_num ? visible_option_num : message_box_data->select.option_num;

    	action_inset_rect.size.width += theme_param->select_field_default_data.option_item_space*(visible_option_num - 1);
    	action_inset_rect.size.height = theme_param->select_field_default_data.option_item.item_size.height;
    	for (index = 0; index < visible_option_num; index++)
    	{
    		action_inset_rect.size.width += theme_param->select_field_default_data.option_item.item_size.width;
    	}

    	action_inset_rect = rect_align(action_inset_rect.size, action_rect, UI_ALIGN_CENTER);
		action_inset_rect.point.x -= painter->offset.x;
		action_inset_rect.point.y -= painter->offset.y;

		text_rect = EMPTY_RECT;
   		text_rect.point.x -= painter->offset.x;
   		text_rect.point.y -= painter->offset.y;
		action_inset_rect.size = theme_param->select_field_default_data.option_item.item_size;
    	for (index = 0; index < visible_option_num; index++)
    	{
    		if (1 == visible_option_num)
    		{
        	    draw_rect_frame_gui(painter, action_inset_rect, \
        	                        theme_param->select_field_default_data.option_item.item_border_size, theme_param->select_field_default_data.option_item.item_border_color, \
									theme_param->select_field_default_data.option_item.item_inset_high_light_color);


        	    text_rect.size = calculate_text_size(message_box_data->select.option_string[message_box_data->select.selected_option], theme_param->select_field_default_data.option_item.font_size);
           		text_rect = rect_align(text_rect.size, action_inset_rect, UI_ALIGN_CENTER);
           	    default_draw_text_param(&param);
           	    param.color = theme_param->select_field_default_data.option_item.item_text_color;
           		param.font_size = theme_param->select_field_default_data.option_item.font_size;
           	    draw_text(painter, text_rect, message_box_data->select.option_string[message_box_data->select.selected_option], -1, &param);

           	    if (theme_param->select_field_default_data.inset_graphic_on_draw)
           	    {
           	    	theme_param->select_field_default_data.inset_graphic_on_draw(message_box, theme_param, action_inset_rect);
           	    }
    			break;
    		}

    		if (index > 0)
    		{
        		action_inset_rect.point.x += (theme_param->select_field_default_data.option_item.item_size.width \
        				+ theme_param->select_field_default_data.option_item_space);
    		}

    	    draw_rect_frame_gui(painter, action_inset_rect, \
    	                        theme_param->select_field_default_data.option_item.item_border_size, theme_param->select_field_default_data.option_item.item_border_color, \
								message_box_data->select.selected_option == index? theme_param->select_field_default_data.option_item.item_inset_high_light_color \
								        			:theme_param->select_field_default_data.option_item.item_inset_normal_color);

    	    text_rect.size = calculate_text_size(message_box_data->select.option_string[index], theme_param->select_field_default_data.option_item.font_size);
       		text_rect = rect_align(text_rect.size, action_inset_rect, UI_ALIGN_CENTER);
       	    default_draw_text_param(&param);
       	    param.color = theme_param->select_field_default_data.option_item.item_text_color;
       		param.font_size = theme_param->select_field_default_data.option_item.font_size;
       	    draw_text(painter, text_rect, message_box_data->select.option_string[index], -1, &param);

       	    if (theme_param->select_field_default_data.inset_graphic_on_draw)
       	    {
       	    	theme_param->select_field_default_data.inset_graphic_on_draw(message_box, theme_param, action_inset_rect);
       	    }
    	}
    }
    else if(message_box_data->msg_box_action_type == INPUT_NUMERIC_TYPE)
    {

    	action_inset_rect.size.width += theme_param->input_field_default_data.input_item_space*(message_box_data->input.digit_num - 1);
        action_inset_rect.size.height = theme_param->input_field_default_data.input_item.item_size.height;
    	for (index = 0; index < message_box_data->input.digit_num; index++)
    	{
    		action_inset_rect.size.width += theme_param->input_field_default_data.input_item.item_size.width;
    	}
    	action_inset_rect = rect_align(action_inset_rect.size, action_rect, UI_ALIGN_CENTER);

		action_inset_rect.point.x -= painter->offset.x;
		action_inset_rect.point.y -= painter->offset.y;

		text_rect = EMPTY_RECT;
   		text_rect.point.x -= painter->offset.x;
   		text_rect.point.y -= painter->offset.y;

		action_inset_rect.size = theme_param->input_field_default_data.input_item.item_size;

    	for (index = 0; index < message_box_data->input.digit_num; index++)
    	{
    		if (index > 0)
    		{
    			action_inset_rect.point.x += theme_param->input_field_default_data.input_item.item_size.width + \
    					theme_param->input_field_default_data.input_item_space;
    		}

		   draw_rect_frame_gui(painter, action_inset_rect, \
										theme_param->input_field_default_data.input_item.border_size, theme_param->input_field_default_data.input_item.border_color, \
										theme_param->input_field_default_data.input_item.item_bkg_color);
		   	if (index < message_box_data->input.input_position)
		   	{
		   		input_text[0] = theme_param->input_field_default_data.numeric_is_show ? message_box_data->input.input_numeric[index] : '*';

		   	}
		   	else
		   	{
		   		input_text[0] = '?';
		   	}

		   	input_text[1] = '\0';
			text_rect.size = calculate_text_size(input_text, theme_param->input_field_default_data.input_item.font_size);
			text_rect = rect_align(text_rect.size, action_inset_rect, UI_ALIGN_CENTER);
			default_draw_text_param(&param);

			param.font_size = theme_param->input_field_default_data.input_item.font_size;
		   	if (index < message_box_data->input.input_position)
			{
				param.color = theme_param->input_field_default_data.input_item.text_high_light_color;
			}
			else
			{
				param.color = theme_param->input_field_default_data.input_item.text_normal_color;
			}
			draw_text(painter, text_rect, input_text, -1, &param);
    	}
    }

    return TRUE;
}


