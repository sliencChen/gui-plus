/*
 * box_sizer.h
 *
 *  Created on: 2019-7-3
 *      Author: Brant
 */

#ifndef GUI_BOX_SIZER_H_
#define GUI_BOX_SIZER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "sizer.h"


typedef UISizer		UIBoxSizer;


typedef struct _ui_box_sizer_data_
{
	U32 dir					:1;
	U32 total_proportion 	:16;
	U32 reserved			:15;
} UIBoxSizerData;


UIBoxSizer* create_box_sizer(int dir);


#ifdef __cplusplus
}
#endif

#endif /* GUI_BOX_SIZER_H_ */
