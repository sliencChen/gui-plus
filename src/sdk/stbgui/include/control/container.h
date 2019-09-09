/*
 * container.h
 *
 *  Created on: 2019-7-5
 *      Author: Brant
 */

#ifndef GUI_CONTAINER_H_
#define GUI_CONTAINER_H_

#ifdef __cplusplus
extern "C"
{
#endif


typedef UIWindow				UIContainer;
#define CONTAINER_CLASS_NAME	"Container"


typedef struct _ui_container_data_
{
	UIInset inset;
	UISizer *sizer;
	U32 auto_size	:1;
	U32 reserved	:31;
} UIContainerData;

void default_container_data(UIContainerData *container_data);
void delete_container_data(UIContainerData *container_data);


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UIContainer* create_container(const char *id);

int set_container_inset(UIContainer *container, UIInset inset);
int set_container_sizer(UIContainer *container, UISizer *sizer);
int set_container_auto_size(UIContainer *container, BOOL auto_size);

int container_add_sub_window(UIContainer *container, UIWindow *sub_win);

/* layout container's sub window, if it has a sizer */
int adjust_container_size(UIContainer *container);
int container_layout_sub_window(UIContainer *container);


#ifdef __cplusplus
}
#endif

#endif /* GUI_CONTAINER_H_ */
