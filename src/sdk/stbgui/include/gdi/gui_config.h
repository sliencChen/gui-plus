/*
 * gui_config.h
 *
 *  Created on: 2018-7-17
 *      Author: Brant
 */

#ifndef _GUI_CONFIG_H_
#define _GUI_CONFIG_H_


/* constant configure */
#define GUI_WINDOW_ID_SIZE		32
#define WINDOW_MAX_NUM			128
#define PAGE_STACK_MAX_NUM		32

//[TODO] reduce this size to save memory, need change it back when we start use gui system. @brant 2018-06-26
#define CLIP_RGN_HEAP_SIZE		1024
#define INVALID_RGN_HEAP_SIZE	1024

/* get search window by function <get_window>, need more memory */
#define ENABLE_SEARCH_WINDOW	0
#define MAIN_WINDOW_STACK_NUM	32
#define MAIN_WINDOW_MAX_NUM		64


/* debug configure */
#define ENABLE_GUI_WARNING		(1)
#define ENABLE_DEBUG_MEMORY		(0)
#define ENABLE_DEBUG_GDI		(0)
#define ENABLE_DEBUG_CLIP_RGN	(0)
#define ENABLE_DEBUG_INV_RGN	(0)



// auto define
//-----------------------------------------------------------------------------
#if ENABLE_GUI_WARNING
#define GUI_WARN(fmt, args...) printf(fmt, ##args)
#else
#define GUI_WARN(fmt, args...)
#endif

#if ENABLE_DEBUG_GDI
#define GDI_LOG(fmt, args...) printf(fmt, ##args)
#else
#define GDI_LOG(fmt, args...)
#endif

#if ENABLE_DEBUG_CLIP_RGN
#define CLIP_LOG(fmt, args...) printf(fmt, ##args)
#else
#define CLIP_LOG(fmt, args...)
#endif

#if ENABLE_DEBUG_INV_RGN
#define INV_LOG(fmt, args...) printf(fmt, ##args)
#else
#define INV_LOG(fmt, args...)
#endif

#endif /* _GUI_CONFIG_H_ */
