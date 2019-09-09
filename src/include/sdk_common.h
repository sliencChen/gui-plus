/*
 * sdk_common.h
 *
 *  Created on: 2019-4-14
 *      Author: Brant
 */

#ifndef SDK_COMMON_H_
#define SDK_COMMON_H_


#include <wx/wx.h>
#include <wx/file.h>
#include "tinyxml.h"
#include "enums.h"


#define DEFAULT_CANVAS_WIDTH	1280
#define DEFAULT_CANVAS_HEIGHT	720
#define DEFAULT_COLOR_MODE		0


#include "uitypes.h"
#include "util.h"
#include "tinywxuni.h"
#include "manager.h"
#include "configmanager.h"
#include "stringmanager.h"

#include "theme/stb_theme.h"
#include "theme/uipagetheme.h"


#define PROJECT_FILE_MAJOR_VERSION		1
#define PROJECT_FILE_MINOR_VERSION		0

#define PAGE_FILE_MAJOR_VERSION			1
#define PAGE_FILE_MINOR_VERSION			0


#endif /* SDK_COMMON_H_ */
