/*
 * box_sizer.h
 *
 *  Created on: 2019-7-3
 *      Author: Brant
 */

#ifndef GUI_LIST_SIZER_H_
#define GUI_LIST_SIZER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "sizer.h"


typedef UISizer		UIListSizer;


typedef struct _ui_list_sizer_data_
{
	U32 dir					:1;
	U32 total_proportion 	:16;
	U32 reserved			:15;
} UIListSizerData;


UIListSizer* create_list_sizer(BOOL dir);


#ifdef __cplusplus
}
#endif

#endif /* GUI_LIST_SIZER_H_ */
