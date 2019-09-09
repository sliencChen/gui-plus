/*
 * arabic_transform.h
 *
 *  Created on: 2016-2-1
 *      Author: Brant
 */

#ifndef _ARABIC_TRANSFORM_H_
#define _ARABIC_TRANSFORM_H_


BOOL is_arabic_transparent_char(U16 char_code);
int arabic_text_transform(U16 *dst, const U16 *src);


#endif /* _ARABIC_TRANSFORM_H_ */
