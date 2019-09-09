/*
 * this file was created by char map tool, please do NOT modify it manually. @brant
 */

#ifndef _FONT_MAX_INFO_
#define _FONT_MAX_INFO_

//#include "gs_global_def.h"


#define FONT_MAX_POINT       101
#define FONT_MAX_TAG         101
#define FONT_MAX_COMPACT_TAG 13

#if ENABLE_THAI_LANGUAGE
#undef FONT_MAX_POINT
#define FONT_MAX_POINT       104
#undef FONT_MAX_TAG
#define FONT_MAX_TAG         104
#undef FONT_MAX_COMPACT_TAG
#define FONT_MAX_COMPACT_TAG 13
#endif
#if ENABLE_CHINESE_LANGUAGE
#undef FONT_MAX_POINT
#define FONT_MAX_POINT       201
#undef FONT_MAX_TAG
#define FONT_MAX_TAG         201
#undef FONT_MAX_COMPACT_TAG
#define FONT_MAX_COMPACT_TAG 26
#endif


#define FONT_MAX_CONTOUR     7

#if ENABLE_CHINESE_LANGUAGE
#undef FONT_MAX_CONTOUR
#define FONT_MAX_CONTOUR     18
#endif


#endif  /* _FONT_MAX_INFO_ */
