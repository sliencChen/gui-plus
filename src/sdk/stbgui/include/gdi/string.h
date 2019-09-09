/*
 * string.h
 *
 *  Created on: 2019-7-16
 *      Author: Brant
 */

#ifndef GUI_STRING_H_
#define GUI_STRING_H_

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct _ui_string_
{
	char *content;
	int size;
} UIString;


int init_string(UIString *string);
int set_string_text(UIString *string, const char *text);
void destroy_string(UIString *string);


#ifdef __cplusplus
}
#endif

#endif /* GUI_STRING_H_ */
