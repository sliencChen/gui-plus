/*
 * resource.h
 *
 *  Created on: 2019-4-25
 *      Author: Brant
 */

#ifndef _GDI_RESOURCE_H_
#define _GDI_RESOURCE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <gui_typedef.h>


//-------------------------------------------------------------------------------------------------
// bitmap
//-------------------------------------------------------------------------------------------------
int set_bmp_manager_info_data(U8 *info, U32 size);
int set_bmp_manager_slice_data(U8 *slice, U32 size);
int bmp_manager_set_mem_pixel_data(U8 *data, U32 size);
int bmp_manager_set_flash_pixel_data(U32 flash_addr, U32 size);
/* for designer */
int set_bmp_manager_bmp_num(int num);
int bmp_manager_add_bmp(int index, const char *name, int width, int height, unsigned int offset, int color_mode, BOOL has_trans);

BOOL gui_get_bmp(const char *name, UIBmp *bmp);
UISize get_bmp_size(const char *name);
BmpSlice *get_bmp_slice(const char *name);
U16 get_bmp_icon_width(const char *name);
U16 get_bmp_icon_height(const char *name);


//-------------------------------------------------------------------------------------------------
// string
//-------------------------------------------------------------------------------------------------
#define MAX_LANGUAGE        100
typedef struct _string_item_
{
	char language[4];
	unsigned int encoding;
	unsigned int original_size;
	unsigned int compress_size;
	unsigned int offset;
	unsigned int check_data;
} StringItem;

typedef struct _string_file_header_
{
	char header_flag[4];	        /* STRG */
	unsigned short version;			/* 0xAABB: AA, major version; BB: micro version */
	unsigned short item_num;
} StringFileHeader;

typedef struct _string_list_manager_
{
	unsigned short item_num;
	StringItem *string_items;

	void *handle;
	U32 (*read)(void *handle, U32 offset, U8 *buf, U32 buf_size);
	void (*close_handle)(void *handle);
} StringListManager;

typedef struct _language_info_
{
	const char *language_string;
	const char *string_mark;
} LanguageInfo;

extern char **all_strings;
extern int str_max_strings;

StringListManager* GetManager(void);
void delete_string_manager(StringListManager *manager);
void load_string_file(const char *filename);

void set_id_language(int id_language);
void set_language(const char *language);
int get_language_index(int language);
int get_language_real_pos(int language);
const char* get_language_name(int language);
const char* get_mark_by_language_index(int language);
int get_language_index_by_string_mark(char *mark);

//-------------------------------------------------------------------------------------------------
// logo - only for designer
//-------------------------------------------------------------------------------------------------
int set_logo_data(int index, unsigned char *data, unsigned int length);


#ifdef __cplusplus
}
#endif

#endif /* _GDI_RESOURCE_H_ */
