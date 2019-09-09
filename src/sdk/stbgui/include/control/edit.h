/*
 * edit.h
 *
 *  Created on: 2019-7-05
 *      Author: Silence
 */


#ifndef _EDIT_H_
#define _EDIT_H_

#ifdef __cplusplus
extern "C"
{
#endif


#include <gui.h>


typedef UIWindow 			UIEdit;
#define EDIT_CLASS_NAME 	CONTAINER_CLASS_NAME "/Edit"

#define EDIT_DEFAULT_WIDTH	900
#define EDIT_DEFAULT_HEIGHT	300


typedef struct _ui_edit_data_
{
	/* inherits from container */
	UIContainerData container;

	//internal use
	UIListSizer *list_sizer;
	int select;
	int current_page;
	int page_max_item;
} UIEditData;

void default_edit_data(UIEditData *edit_data);


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UIEdit* create_edit(const char *id);
int set_edit_inset(UIEdit* edit, UIInset inset);
int set_edit_max_item(UIEdit* edit, int max_item);
int set_edit_sizer(UIEdit *edit);

BOOL edit_add_window(UIEdit *edit, UIWindow *sub_win);


//-------------------------------------------------------------------------------------------------
// typical theme
//-------------------------------------------------------------------------------------------------
typedef UIMenuThemeDefaultData  UIEditThemeDefaultData;

#ifdef __cplusplus
}
#endif

#endif // _EDIT_H_
