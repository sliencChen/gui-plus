/*
 * popup.h
 *
 *  Created on: 2018-7-6
 *      Author: Brant
 */

#ifndef GUI_POPUP_H_
#define GUI_POPUP_H_

#ifdef __cplusplus
extern "C"
{
#endif


#include <control/container.h>


typedef UIWindow 			UIPopup;
#define POPUP_CLASS_NAME	CONTAINER_CLASS_NAME "/Popup"


typedef struct _ui_popup_data_
{
	/* inherits from container */
	UIContainerData container;

	/* special for popup */
	UIString title;
	U32 centered	:1;
	U32 reserved	:31;
} UIPopupData;

void default_popup_data(UIPopupData *popup_data);
void delete_popup_data(UIPopupData *popup_data);


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UIPopup* create_popup(const char *id);

int set_popup_title(UIPopup *popup, const char *text);
int set_popup_inset(UIPopup *popup, UIInset inset);
int set_popup_sizer(UIPopup *popup, UISizer *sizer);
int set_popup_centered(UIPopup *popup, BOOL centered);

BOOL popup_add_window(UIPopup *popup, UIWindow *sub_win);


//-------------------------------------------------------------------------------------------------
// typical theme
//-------------------------------------------------------------------------------------------------
typedef struct _popup_theme_default_
{
	UIColor title_color;
	UIColor title_bkg_color;
	UIColor border_color;
	UIColor inside_bkg_color;
	int border_size;
	int title_height;
	int font_size;
} UIPopupThemeDefaultData;

BOOL popup_theme_default(UIPopup* popup, UIPopupThemeDefaultData *theme_param);


#ifdef __cplusplus
}
#endif

#endif /* GUI_POPUP_H_ */
