/*
 * string.c
 *
 *  Created on: 2019-7-16
 *      Author: Brant
 */

#include <gdi/gui_interface.h>
#include <gdi/string.h>


int init_string(UIString *string)
{
	if (string == NULL)
		return -1;

	string->content = NULL;
	string->size = 0;

	return 0;
}

int set_string_text(UIString *string, const char *text)
{
	int text_len = gui_strlen(text);

	/* need more space */
	if (string->content == NULL || string->size <= text_len)
	{
		destroy_string(string);
		string->content = (char *)gui_malloc((text_len + 1) * sizeof(char));
		if (string->content == NULL)
			return -1;

		string->size = text_len + 1;
	}

	gui_strcpy(string->content, text);

	return text_len;
}

void destroy_string(UIString *string)
{
	if (string->content != NULL)
	{
		gui_free(string->content);
		string->content = NULL;
		string->size = 0;
	}
}
