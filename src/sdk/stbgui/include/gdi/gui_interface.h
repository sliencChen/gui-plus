/*
 * gui_interface.h
 *
 *  Created on: 2017-12-27
 *      Author: Brant
 */

#ifndef _GUI_INTERFACE_H_
#define _GUI_INTERFACE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

typedef char* bmp_icon_type;
typedef enum
{
	COVER_BACKGROUND = 0,
	MIX_BACKGROUND,
	BLEND_BACKGROUND,
} DRAW_MODE;

typedef enum
{
	//auto type
	ICON_TYPE_BMP,
	ICON_TYPE_COLOR,
	ICON_TYPE_TEXT,		//like [F1]
	//user type
	ICON_TYPE_EXTRA,	//extra bmp, like BMP_MV in NET_PHOTO_SKIN
	ICON_TYPE_MAX,
}ICON_TYPE;
typedef char* string_table;


#include "gui_typedef.h"
//#include "gs_global_def.h"
//#include "gs_os_interface.h"
//#include "usif.h"
#include "rect.h"
#include "color.h"
//#include "utility.h"
//#include "gs_osd_interface.h"
#include "font/conv.h"
#include "gui_config.h"


#define GS_ERROR printf

/* functions */
#if ENABLE_DEBUG_MEMORY
void* gui_malloc_internal(const char *invoker, unsigned int size);
void gui_free_internal(void *addr);
void print_memory_use(void);

#define gui_malloc(x)	gui_malloc_internal(__FUNCTION__, x)
#define gui_free(x)		gui_free_internal(x)
#else
#define gui_malloc		malloc
#define gui_free		free
#endif

#define gui_memset		memset
#define gui_memcpy		memcpy
#define gui_memcmp		memcmp
#define gui_memmove		memmove

#define gui_strlen		strlen
#define gui_strcpy		strcpy
#define gui_strcmp		strcmp
#define gui_strncmp		strncmp
#define gui_printf		printf
#define gui_snprintf	snprintf


#define GUI_MUTEX_WAIT_FOREVER	    (0xFFFFFFFF)
#define GUI_INVALID_MUTEX_HANDLE    (-1)

int gui_mutex_init(int *handle, char *name);
int gui_mutex_delete(int *handle);
int gui_mutex_lock(int *handle, U32 timeout);
int gui_mutex_unlock(int *handle);


#define GUI_MSG_WAIT_FOREVER	0xFFFFFFFF

int gui_message_send(U32 msg_id, U32 param1, U32 param2, U32 param3);
int gui_message_receive(U32 *msg_id, U32 *param1, U32 *param2, U32 *param3, U32 timeout);


#endif /* _GUI_INTERFACE_H_ */
