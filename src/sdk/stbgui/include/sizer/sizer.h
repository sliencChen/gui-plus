/*
 * sizer.h
 *
 *  Created on: 2019-7-2
 *      Author: Brant
 */

#ifndef GUI_SIZER_H_
#define GUI_SIZER_H_

#ifdef __cplusplus
extern "C"
{
#endif


//-------------------------------------------------------------------------------------------------
// UISizerItem
//-------------------------------------------------------------------------------------------------
enum
{
	UI_SIZER_ITEM_WINDOW,
	UI_SIZER_ITEM_SPACE,
	UI_SIZER_ITEM_SIZER,
	UI_SIZER_ITEM_MAX
};

enum
{
	UI_SIZER_FLAG_EXPAND		= 0x010,
	UI_SIZER_FLAG_SHAPED		= 0x020,
	UI_SIZER_FLAG_BORDER_LEFT	= 0x040,
	UI_SIZER_FLAG_BORDER_TOP	= 0x080,
	UI_SIZER_FLAG_BORDER_RIGHT	= 0x100,
	UI_SIZER_FLAG_BORDER_BOTTOM = 0x200,
};


typedef struct _ui_sizer_item_
{
	U32 type			:2;
	U32 flags			:10;
	U32 reserved		:20;

	UISize min_size;
	U16 proportion;
	U16 border;

	union
	{
		struct _ui_window_ *window;
		struct _ui_sizer_ *sizer;
	};

	UISize min_size_temp;
} UISizerItem;


BOOL sizer_item_shown(UISizerItem *item);
UISize get_sizer_item_min_size_with_border(UISizerItem *item);
UISize get_sizer_item_max_size_with_border(UISizerItem *item);
UISize sizer_item_calc_min_size(UISizerItem *item);
BOOL inform_sizer_item_first_dir(UISizerItem *item, int direction, int size, int availableOtherDir);
void set_sizer_item_dimension(UISizerItem *item, UIPoint point, UISize size);


//-------------------------------------------------------------------------------------------------
// UISizerItemNode
//-------------------------------------------------------------------------------------------------
typedef struct _ui_sizer_item_node_
{
	UISizerItem item;
	struct _ui_sizer_item_node_ *next;
} UISizerItemNode;


void destroy_sizer_item_node(UISizerItemNode *node);


//-------------------------------------------------------------------------------------------------
// UISizer
//-------------------------------------------------------------------------------------------------
enum
{
	UI_SIZER_DIR_HOR = 0,
	UI_SIZER_DIR_VER = 1
};

typedef struct _ui_sizer_
{
	UIPoint position;
	UISize size;
	UISize min_size;
	void *private_data;
	UISizerItemNode *items;
	UISize (*calc_min_size)(struct _ui_sizer_ *self);
	void (*reposition_children)(struct _ui_sizer_ *self, UISize min_size);
	void (*on_destroy)(struct _ui_sizer_ *self);
} UISizer;


UISizer* create_sizer(void);
void destroy_sizer(UISizer *sizer);

int sizer_add_window(UISizer *sizer, UIWindow *window, UISize min_size, int proportion, int border, U32 flags);
int sizer_add_sub_sizer(UISizer *sizer, UISizer *sub_sizer, int proportion, int border, U32 flags);

int get_item_count(UISizer *sizer);

BOOL any_item_show_in_sizer(UISizer *sizer);
UISize calc_sizer_min_size(UISizer *self);
UISize get_sizer_min_size(UISizer *self);

void sizer_set_dimension(UISizer *sizer, UIRect rect);
void sizer_layout(UISizer *sizer);


//-------------------------------------------------------------------------------------------------
// UISizer - util
//-------------------------------------------------------------------------------------------------
int GetMinOrRemainingSize(int orient, UISizerItem *item, int *remaining_space);


#ifdef __cplusplus
}
#endif

#endif /* GUI_SIZER_H_ */
