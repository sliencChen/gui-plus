/*
 * edit.h
 *
 *  Created on: 2019-7-05
 *      Author: Silence
 */

#ifndef _PROMPT_H_
#define _PROMPT_H_

#ifdef __cplusplus
extern "C"
{
#endif


#include <gui.h>


typedef UIWindow				UIPrompt;
#define PROMPT_CLASS_NAME		POPUP_CLASS_NAME "/Prompt"

#define PROMPT_DEFAULT_WIDTH	300
#define PROMPT_DEFAULT_HEIGHT	150

typedef struct _ui_prompt_data_
{
	UIPopupData popup;

	/* special for prompt */
	UIString content;
	int duration;       /*millisecond*/

	/* internal use */
	UITextField *text_field;
	UIBoxSizer *prompt_sizer;
}UIPromptData;




void default_prompt_data(UIPromptData *prompt_data);
void delete_prompt_data(UIPromptData *prompt_data);


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UIPrompt* create_prompt(const char *id);
int set_prompt_title(UIPrompt *prompt,const char *text);
int set_prompt_content(UIPrompt *prompt,const char *text);
int	set_prompt_druation(UIPrompt *prompt, int duration_ms);
int set_prompt_centered(UIPrompt *popup, BOOL centered);
int set_prompt_sizer(UIPrompt *prompt, UISizer *sizer);
BOOL prompt_add_window(UIPrompt *prompt, UIWindow *sub_win);


//-------------------------------------------------------------------------------------------------
// typical theme
//-------------------------------------------------------------------------------------------------
typedef UIPopupThemeDefaultData	 UIPromptThemeDefaultData;


#ifdef __cplusplus
}
#endif

#endif//_PROMPT_H_
