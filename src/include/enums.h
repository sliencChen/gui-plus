/*
 * enums.h
 *
 *  Created on: 2019-4-17
 *      Author: Brant
 */

#ifndef _ENUMS_H_
#define _ENUMS_H_


enum
{
	ColorMode_ARGB8888 = 0,
	ColorMode_RGB565,
	ColorMode_ARGB1555,
	ColorMode_Auto = 0xffff,		/* for images, use same color mode as project */
	ColorMode_Max,
};


#endif /* _ENUMS_H_ */
