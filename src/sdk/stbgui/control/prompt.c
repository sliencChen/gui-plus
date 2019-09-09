/*
 * edit.c
 *
 *  Created on: 2019-7-17
 *      Author: Silence
 */

#include <gui_window.h>
#include <control/text_field.h>
#include <control/prompt.h>


//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// internal use
//-------------------------------------------------------------------------------------------------
static void delete_prompt_private_data(UIPromptData *prompt_data)
{
	if (prompt_data)
	{
		delete_prompt_data(prompt_data);
		gui_free(prompt_data);
	}
}

static UIPromptData *new_prompt_private_data(void)
{
	UIPromptData *prompt_data;

	prompt_data = (UIPromptData *)gui_malloc(sizeof(UIPromptData));
	if (prompt_data == NULL)
	{
		return NULL;
	}

	default_prompt_data(prompt_data);
	prompt_data->text_field = create_text_field("PromptText");
	prompt_data->prompt_sizer = create_box_sizer(UI_SIZER_DIR_VER);

	return prompt_data;
}


static int set_prompt_string_internal(UIString *string, const char *text)
{
	return set_string_text(string, text);
}



//-------------------------------------------------------------------------------------------------
// window manager callback
//-------------------------------------------------------------------------------------------------
static BOOL prompt_on_destroy(UIPrompt *self)
{
	delete_prompt_private_data(self->private_data);
	return TRUE;
}


static BOOL prompt_on_draw(UIPrompt *self)
{
	UIPromptData *prompt_data = (UIPromptData *)self->private_data;
    UIPopupThemeDefaultData default_prompt;

    default_prompt.title_color = RED_COLOR;
    default_prompt.title_bkg_color = 0xFF9BCD9B;
    default_prompt.border_color = 0xFF9932CC;
    default_prompt.inside_bkg_color = 0xFF98F5FF;
    default_prompt.border_size = 2;
    default_prompt.title_height = 60;
    default_prompt.font_size = -1;
	set_text_field_text_color(prompt_data->text_field, BLACK_COLOR);
	return popup_theme_default(self, &default_prompt);
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
BOOL prompt_add_window(UIPrompt *prompt, UIWindow *sub_win)
{
	if ( !prompt || !prompt->private_data || !sub_win )
	{
		return FALSE;
	}
    add_sub_window(prompt, sub_win);

	return TRUE;
}


void default_prompt_data(UIPromptData *prompt_data)
{
	default_popup_data(&(prompt_data->popup));

	init_string(&(prompt_data->content));
	prompt_data->duration = 0;
	prompt_data->text_field = NULL;
	prompt_data->prompt_sizer = NULL;
}


void delete_prompt_data(UIPromptData *prompt_data)
{
	delete_popup_data(&(prompt_data->popup));
	destroy_string(&(prompt_data->content));
}


int set_prompt_centered(UIPrompt *prompt, BOOL centered)
{
	return set_popup_centered(prompt, centered);
}


int set_prompt_sizer(UIPrompt *prompt, UISizer *sizer)
{
	UIPromptData *prompt_data;
	UIRect prompt_rect;
	if (!prompt || !prompt->private_data)
	{
		return -1;
	}

	prompt_data = (UIPromptData *)prompt->private_data;
	set_container_sizer(prompt, sizer);
	if (prompt_data->popup.centered)
	{
		prompt_rect = rect_align(prompt->rect.size, ui_rect(0, 0, main_canvas_width(), main_canvas_height()), UI_ALIGN_CENTER);
		prompt_rect.point = screen_to_client(prompt->parent, prompt_rect.point);
		set_window_rect(prompt, prompt_rect);
	}
    return 0;
}


UIPrompt* create_prompt(const char *id)
{
	UIPrompt *prompt;

	prompt = create_sub_window(id, 0, 0, PROMPT_DEFAULT_WIDTH, PROMPT_DEFAULT_HEIGHT);
	if (prompt)
	{
        UIPromptData *prompt_data = new_prompt_private_data();
        if (prompt_data == NULL)
		{
			destroy_window(prompt);
			return NULL;
		}

        prompt->private_data = prompt_data;
        prompt->focus_stop = TRUE;
        prompt->class_name = PROMPT_CLASS_NAME;
        prompt->on_draw = prompt_on_draw;
        prompt->on_destroy = prompt_on_destroy;
        if (prompt_data->text_field)
        {
        	add_sub_window(prompt, prompt_data->text_field);
        	if (prompt_data->prompt_sizer)
        	{
            	sizer_add_window(prompt_data->prompt_sizer, prompt_data->text_field, ui_size(200, 40), 0, 0, UI_ALIGN_CENTER_VER);
        	}
        }
	}
	return prompt;
}


int set_prompt_title(UIPrompt *prompt,const char *text)
{
	UIPromptData *prompt_data;
	if (!prompt || !prompt->private_data || !text)
	{
		return -1;
	}

	prompt_data = (UIPromptData *)prompt->private_data;
	set_prompt_string_internal(&prompt_data->popup.title, text);
	invalidate_window(prompt);
	return 0;
}


int set_prompt_content(UIPrompt *prompt,const char *text)
{
	UIPromptData *prompt_data;
	int line_height;
	int line_num;
	UISize mini_size;
	UISize text_size;
	if (!prompt || !prompt->private_data || !text)
	{
		return -1;
	}

	prompt_data = (UIPromptData *)prompt->private_data;
	set_prompt_string_internal(&prompt_data->content, text);
	if (prompt_data->text_field)
	{
		/*adjust the text field (now just can adjust the height)*/
		text_size = ui_size(prompt->rect.size.width, prompt->rect.size.height - 80);
		set_window_rect(prompt_data->text_field, ui_rect(0, 0, text_size.width, text_size.height));
		set_text_field_text(prompt_data->text_field, prompt_data->content.content, 0);
		line_num = get_text_field_line_num(prompt_data->text_field);
		line_height = get_text_field_line_height(prompt_data->text_field);
		mini_size = ui_size(text_size.width, line_num*line_height);
		prompt_data->text_field->min_size = mini_size;
		set_text_field_align_center(prompt_data->text_field, TRUE);
		set_container_inset(prompt, ui_inset(20, 80, 20, 20));
		if (prompt_data->prompt_sizer)
		{
			set_prompt_sizer(prompt, prompt_data->prompt_sizer);
		}
	}
	invalidate_window(prompt);
	return 0;
}


int	set_prompt_druation(UIPrompt *prompt, int duration_ms)
{
	UIPromptData *prompt_data;
	if (!prompt || !prompt->private_data)
	{
		return -1;
	}
	prompt_data = (UIPromptData *)prompt->private_data;
	prompt_data->duration = duration_ms;
	invalidate_window(prompt);
	return 0;
}


//-------------------------------------------------------------------------------------------------
// typical theme
//-------------------------------------------------------------------------------------------------

