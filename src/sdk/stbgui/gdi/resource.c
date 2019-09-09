/*
 * resource.c
 *
 *  Created on: 2019-4-25
 *      Author: Brant
 */

#include <gui_common.h>
#include <gdi/bitmap.h>
#include <gdi/resource.h>
#include "../../platform/compress.h"



#define ALIGN_MEM_WITH_UNIT(n, unit)	((((n)+(unit)-1)/(unit))*(unit))


//-------------------------------------------------------------------------------------------------
// Bitmap - type define
//-------------------------------------------------------------------------------------------------
typedef struct _bmp_item_
{
	char	name[32];		// bmp name
	U32		addr;			// bmp offset in this data
	U32		length;			// bmp len in this data
	U32 	original_len;	// bmp len before compress, 0 is not compress
	U16 	width;			// bmp width(Pixel)
	U16		height;			// bmp height(Pixel)
	U32		color_mode:4;	// 0:ARGB8888, 1:RGB565, 2:ARGB1555, 3:RGB888
	U32		has_trans:1;
	U32		reserved:27;
} BmpItem;

typedef struct _bmp_slice_item_
{
	unsigned int bmp_index;
	BmpSlice slice;
} BmpSliceItem;

typedef struct _data_reader_
{
	void* handle;		///< handle for read data, can be memory, flash addr
	U8* (*read)(struct _data_reader_ *reader, U32 offset, U8 *buf, U32 length);
	void (*close_handle)(struct _data_reader_ *reader);
} DataReader;

typedef struct _bmp_manager_
{
	U16 bmp_num;		///< number of bmps
	BmpItem *bmps;		///< all bmp item info
	U16 slice_num;
	BmpSliceItem *slices;
	U8 *read_buf;		///< if read bmp from ROM, need a buf for save data. set to NULL if bmp data is from mem.
	U8 *uncompr_buf;	///< if one or more bmp is compressed, alloc a buf for uncompress.
	DataReader reader;
} BmpManager;

typedef struct _bmp_info_data_header_
{
	U8	version;	///< info data version
	U8	length;		///< header length, include version & length
	U16	bmp_num;	///< number of bmp in info data
} BmpInfoDataHeader;

enum
{
	BMP_PIXEL_DATA_FROM_MEM,
	BMP_PIXEL_DATA_FROM_FLASH,
	BMP_PIXEL_DATA_TYPE_MAX
};


//-------------------------------------------------------------------------------------------------
// Bitmap - local variable
//-------------------------------------------------------------------------------------------------
static BmpManager global_bmp_manager;


//-------------------------------------------------------------------------------------------------
// Bitmap - manager
//-------------------------------------------------------------------------------------------------
static int alloc_bmp_uncompress_buf_if_needed(BmpManager *manager)
{
	unsigned int i, max_original_len = 0;
	const int more_space = 24;

	if (manager->uncompr_buf)
	{
		gui_free(manager->uncompr_buf);
		manager->uncompr_buf = NULL;
	}

	for (i = 0; i < manager->bmp_num; i++)
	{
		max_original_len = MAX(max_original_len, manager->bmps[i].original_len);
//		printf("%s\n", manager->bmps[i].name);
	}
	if (max_original_len > 0)
	{
		manager->uncompr_buf = gui_malloc(ALIGN_MEM_WITH_UNIT(max_original_len, 4) + more_space);
		if (manager->uncompr_buf == NULL)
			return -1;
	}
	return 0;
}

static int bmp_manager_parse_info_data(BmpManager *manager, U8 *slice, U32 size)
{
	BmpInfoDataHeader header;
	unsigned int bmp_item_data_size;

	/* check data header, need refine it later @brant 2019-06-24 */
	gui_memcpy(&header, slice, sizeof(BmpInfoDataHeader));
	if (header.length != sizeof(BmpInfoDataHeader))
		return -1;

	/* check bmp item data */
	manager->bmp_num = header.bmp_num;
	bmp_item_data_size = header.bmp_num * sizeof(BmpItem);
	if (size - header.length < bmp_item_data_size)	/* no enough data */
		return -2;

	if (manager->bmps != NULL)
		gui_free(manager->bmps);

	manager->bmps = (BmpItem *)gui_malloc(bmp_item_data_size);
	if (manager->bmps == NULL)
		return -3;

	gui_memcpy(manager->bmps, slice + header.length, bmp_item_data_size);
	if (alloc_bmp_uncompress_buf_if_needed(manager) != 0)
	{
		gui_free(manager->bmps);
		manager->bmps = NULL;
		return -4;
	}
	return 0;
}

static int bmp_manager_parse_slice_data(BmpManager *manager, U8 *slice, U32 size)
{
	BmpInfoDataHeader header;
	unsigned int slice_item_data_size;

	/* check data header, need refine it later @brant 2019-06-24 */
	gui_memcpy(&header, slice, sizeof(BmpInfoDataHeader));
	if (header.length != sizeof(BmpInfoDataHeader))
		return -1;

	/* check bmp item data */
	manager->slice_num = header.bmp_num;
	slice_item_data_size = header.bmp_num * sizeof(BmpSliceItem);
	if (size - header.length < slice_item_data_size)	/* no enough data */
		return -2;

	if (manager->bmps != NULL)
		gui_free(manager->slices);

	manager->slices = (BmpSliceItem *)gui_malloc(slice_item_data_size);
	if (manager->slices == NULL)
		return -3;

	gui_memcpy(manager->slices, slice + header.length, slice_item_data_size);

	return 0;
}

U8* read_mem(struct _data_reader_ *reader, U32 offset, U8 *buf, U32 length)
{
	U8 *pixel_data_mem = (U8 *)reader->handle;

	if (!buf)
		return pixel_data_mem + offset;

	gui_memcpy(buf, pixel_data_mem + offset, length);
	return buf;
}

U8* read_flash(struct _data_reader_ *reader, U32 offset, U8 *buf, U32 length)
{
//	U32 flash_start = (U32)reader->handle;

	if (buf)
	{
//		flash_read(flash_start + offset, buf, length);
	}
	return buf;
}

static int bmp_manager_set_pixel_data(BmpManager *manager, U8 *data, U32 size, int type)
{
	unsigned int i, item_data_max_size = 0;
	unsigned int data_end, item_data_max_end = 0;

	if (manager->bmp_num == 0 || manager->bmps == NULL)
	{
		gui_printf("should call set_bmp_manager_info_data first.\n");
		return -1;
	}
	for (i = 0; i < manager->bmp_num; i++)
	{
		item_data_max_size = MAX(item_data_max_size, manager->bmps[i].length);
		data_end = manager->bmps[i].addr + manager->bmps[i].length;
		item_data_max_end = MAX(item_data_max_end, data_end);
	}
	if (item_data_max_end > size)
	{
		gui_printf("no enough data. pointer end at 0x%08X, but only 0x%08X bytes data avaliable.\n", item_data_max_end, size);
		return -2;
	}

	if (manager->read_buf)
		gui_free(manager->read_buf);

	manager->read_buf = NULL;
	manager->reader.handle = data;
	/* alloc a read buf to avoid do it everytime */
	if (type == BMP_PIXEL_DATA_FROM_FLASH)
	{
		manager->read_buf = (unsigned char *)gui_malloc(item_data_max_size);
		if (manager->read_buf == NULL)
			return -3;

		manager->reader.read = read_flash;
	}
	else
	{
		manager->reader.read = read_mem;
	}

	return 0;
}


//-------------------------------------------------------------------------------------------------
// Bitmap - get
//-------------------------------------------------------------------------------------------------
static int bmp_manager_get_bmp_index(BmpManager *manager, const char *name)
{
	int	low = 0, mid, high;
	int	cmp_res;

	if (!manager || !manager->bmps || manager->bmp_num == 0 || name == NULL || strlen(name) == 0)
		return -1;

	high = manager->bmp_num;
	while (low <= high)
	{
		mid = (low + high) / 2;
		cmp_res = gui_strcmp(name, manager->bmps[mid].name);

		if (cmp_res == 0)
			return mid;
		else if (cmp_res > 0)
			low = mid + 1;
		else
			high = mid - 1;
	}

	return -1;
}

static BmpItem* bmp_manager_get_bmp_info(BmpManager *manager, const char *name)
{
	int bmp_index;

	bmp_index = bmp_manager_get_bmp_index(manager, name);
	if (bmp_index < 0)
		return NULL;

	return manager->bmps + bmp_index;
}

static BmpSlice* bmp_manager_get_bmp_slice(BmpManager *manager, const char *name)
{
	int bmp_index;
	int	low = 0, mid, high;

	if (!manager || !manager->slices || manager->slice_num <= 0)
		return NULL;

	bmp_index = bmp_manager_get_bmp_index(manager, name);
	if (bmp_index < 0)
		return NULL;

	high = manager->slice_num;
	while (low <= high)
	{
		mid = (low + high) / 2;

		if (bmp_index == manager->slices[mid].bmp_index)
			return &(manager->slices[mid].slice);
		else if (bmp_index == manager->slices[mid].bmp_index)
			low = mid + 1;
		else
			high = mid - 1;
	}

	return NULL;
}

static BOOL load_bmp_pixel_data(BmpManager *manager, BmpItem *info_item, UIBmp *bmp)
{
	U8 *pixel_data = manager->reader.read(&(manager->reader), info_item->addr, manager->read_buf, info_item->length);
	unsigned int original_size = 0;

	if (pixel_data == NULL)
		return FALSE;

	if (info_item->original_len > 0)	/* need uncompress */
	{
		if (manager->uncompr_buf)
		{
			uncompress_data(manager->uncompr_buf, &original_size, pixel_data, info_item->length);
			pixel_data = manager->uncompr_buf;
		}
	}

	create_pixel_buf(&(bmp->pixel_buf), pixel_data, ui_size(info_item->width, info_item->height), info_item->color_mode);
	bmp->has_trans = info_item->has_trans;

	return TRUE;
}

static BOOL bmp_manager_get_bmp(BmpManager *manager, const char *name, UIBmp *bmp)
{
	BmpItem *info;

	info = bmp_manager_get_bmp_info(manager, name);
	if (info == NULL)
		return FALSE;

	return load_bmp_pixel_data(manager, info, bmp);
}


//-------------------------------------------------------------------------------------------------
// bitmap - for designer
//-------------------------------------------------------------------------------------------------
int set_bmp_manager_bmp_num(int num)
{
	global_bmp_manager.bmp_num = num;
	if (global_bmp_manager.bmps)
		gui_free(global_bmp_manager.bmps);

	global_bmp_manager.bmps = gui_malloc(global_bmp_manager.bmp_num * sizeof(BmpItem));
	if (global_bmp_manager.bmps == NULL)
		return -1;

	global_bmp_manager.read_buf = NULL;
	global_bmp_manager.uncompr_buf = NULL;
	gui_memset(&(global_bmp_manager.reader), 0, sizeof(DataReader));

	return 0;
}

int bmp_manager_add_bmp(int index, const char *name, int width, int height, unsigned int offset, int color_mode, BOOL has_trans)
{
	BmpItem *item;

	if (index < 0 || index >= global_bmp_manager.bmp_num)
		return -1;

	item = global_bmp_manager.bmps + index;
	gui_snprintf(item->name, sizeof(item->name), "%s", name);
	item->width = width;
	item->height = height;
	item->addr = offset;
	item->length = width * height * get_color_pixel_size(color_mode);
	item->original_len = 0;
	item->color_mode = color_mode;
	item->has_trans = has_trans;

	return 0;
}

void bmp_manager_add_bmp_finish()
{
	int i;
	unsigned int addr = 0;

	for (i = 0; i < global_bmp_manager.bmp_num; i++)
	{
		global_bmp_manager.bmps[i].addr = addr;
		addr += ALIGN_MEM_WITH_UNIT(global_bmp_manager.bmps[i].length, 4);
	}

	alloc_bmp_uncompress_buf_if_needed(&global_bmp_manager);
}

//-------------------------------------------------------------------------------------------------
// bitmap - interface
//-------------------------------------------------------------------------------------------------
int set_bmp_manager_info_data(U8 *info, U32 size)
{
	return bmp_manager_parse_info_data(&global_bmp_manager, info, size);
}

int set_bmp_manager_slice_data(U8 *slice, U32 size)
{
	return bmp_manager_parse_slice_data(&global_bmp_manager, slice, size);
}

int bmp_manager_set_mem_pixel_data(U8 *data, U32 size)
{
	return bmp_manager_set_pixel_data(&global_bmp_manager, data, size, BMP_PIXEL_DATA_FROM_MEM);
}

int bmp_manager_set_flash_pixel_data(U32 flash_addr, U32 size)
{
	return bmp_manager_set_pixel_data(&global_bmp_manager, (U8 *)flash_addr, size, BMP_PIXEL_DATA_FROM_FLASH);
}

BOOL gui_get_bmp(const char *name, UIBmp *bmp)
{
	return bmp_manager_get_bmp(&global_bmp_manager, name, bmp);
}

UISize get_bmp_size(const char *name)
{
	BmpItem *bmp_item = NULL;
	UISize size = {0, 0};

	bmp_item = bmp_manager_get_bmp_info(&global_bmp_manager, name);
	if (bmp_item)
		size = ui_size(bmp_item->width, bmp_item->height);

	return size;
}

BmpSlice *get_bmp_slice(const char *name)
{
	return bmp_manager_get_bmp_slice(&global_bmp_manager, name);
}

U16 get_bmp_icon_width(const char *name)
{
	return get_bmp_size(name).width;
}

U16 get_bmp_icon_height(const char *name)
{
	return get_bmp_size(name).height;
}

BOOL check_extra_bmp(bmp_icon_type name, int type)
{
#if 0
#if 1//GET_BMP_BY_NAME
	if (check_circle_bmp(name, type))
	{
		return TRUE;
	}
	else if (conver_bmp_to_text_gui(NULL, 0, name) > 0)
	{
		return TRUE;
	}
#else
	if (name > BMP_MAX && name <= BMP_YELLOW_CIRCLE)
	{
		return TRUE;
	}
	else if (name > BMP_YELLOW_CIRCLE && name < BMP_REAL_MAX)
	{
		return TRUE;
	}
#endif
#endif
	return FALSE;
}

BOOL check_bmp_valid(bmp_icon_type name, int type)
{
#if 1//GET_BMP_BY_NAME
	if (bmp_manager_get_bmp_info(&global_bmp_manager, name))
	{
		return TRUE;
	}
	else if (type == 1 && check_extra_bmp(name, 0))
	{
		return TRUE;
	}
#else
	if (type == 0 && name < BMP_MAX)
	{
		return TRUE;
	}
	else if (type == 1 && name < BMP_REAL_MAX)
	{
		return TRUE;
	}
#endif

	return FALSE;
}


//-------------------------------------------------------------------------------------------------
// String -- best have id language
//-------------------------------------------------------------------------------------------------
static LanguageInfo language_infos[] =
{
	{"English",		"en"},
	{"Deutsch",		"de"},
	{"Русский",		"ru"},
	{"العربية",			"ar"},
	{"Magyar",		"hu"},
	{"Türkçe",		"tr"},
	{"Italiano",	"it"},
	{"Français",	"fr"},
	{"Español",		"es"},
	{"فارسى",			"fa"},
	{"Polski",		"pl"},
	{"Ελλινικά",	"el"},
	{"České",		"cs"},
	{"Slovenský",	"sk"},
	{"Український",	"uk"},
	{"Svenska",		"sv"},
	{"Nederlands",	"nl"},
	{"ภาษาไทย",		"th"},
	{"slovenski",	"sl"},
	{"Български",	"bg"},
	{"Român",		"ro"},
	{"Português",	"pt"},
	{"简体中文",	"zh"},
	{"Dansk",		"da"},
	{"Монгол Улсын","mn"},
	{"Norsk språk",	"no"},
	{"Suomi",		"fi"},
	{"Hrvatska",	"hr"},
	{"ភាសាខ្មែរ",		 "km"},
	{"조선어",	        "ko"},
};
static U16 language_index[MAX_LANGUAGE];
static StringListManager *global_string_manager = NULL;
char **all_strings;
static char *string_buffer_for_cur_language;
static char *string_buffer_for_id_language;
static BOOL is_id_language;
int str_max_strings;

static BOOL IsValidLanguage(int language)
{
	return (language >= 0 && language < MAX_LANGUAGE);
}

StringListManager* GetManager(void)
{
	return global_string_manager;
}

int get_language_index(int language)
{
	return language_index[language];
}

static LanguageInfo* get_language_info(int language)
{
	int iLoop;

	for (iLoop = 0; iLoop < sizeof(language_infos) / sizeof(language_infos[0]); iLoop++)
	{
		if (iLoop == language)
		{
			return language_infos + iLoop;
		}
	}
	gui_printf("no such language, id: %d\n", language);

	return NULL;
}

const char* get_language_name(int language)
{
	LanguageInfo *info;

	info = get_language_info(language);
	if (info == NULL)
	{
		return "Unsupported";
	}
	return info->language_string;
}

/*static*/ int get_language_index_by_string_mark(char *mark)
{
	int iLoop;

	for (iLoop = 0; iLoop < sizeof(language_infos) / sizeof(language_infos[0]); iLoop++)
	{
		if (strcmp(language_infos[iLoop].string_mark, mark) == 0)
		{
			return iLoop;
		}
	}
	return -1;
}

const char* get_mark_by_language_index(int language)
{
    int iLoop;

    for (iLoop = 0; iLoop < sizeof(language_infos) / sizeof(language_infos[0]); iLoop++)
	{
		if (iLoop == language)
		{
			return language_infos[iLoop].string_mark;
		}
	}
	gui_printf("no such language, id: %d\n", language);
	return "No Such Language";
}

int get_language_real_pos(int language)
{
	int iPos = -1;
	int iLoop;

	for (iLoop = 0; iLoop < MAX_LANGUAGE; iLoop++)
	{
		if ((language_index[iLoop] & 0xff) == language)
		{
			iPos = iLoop;
			break;
		}
	}

	return iPos;
}

static U32 read_string_file(void *handle, U32 offset, U8 *buf, U32 length)
{
	FILE *file = (FILE *)handle;

	fseek(file, offset, SEEK_SET);
	return fread(buf, 1, length, file);
}

static void close_string_file(void *handle)
{
	FILE *file = (FILE *)handle;

	fclose(file);
}

void delete_string_manager(StringListManager *manager)
{
	if (manager)
	{
		if (manager->close_handle)
			manager->close_handle(manager->handle);

		if (manager->string_items)
			gui_free(manager->string_items);

		gui_free(manager);
	}
}


static U32 get_string_check_data(U8 *data, U32 length)
{
	int i;
	U32 check_data = 0;

	for (i = 0; i < length; i++)
	{
		check_data += data[i];
	}

	return check_data;
}

static U8* get_string_item_data(StringListManager *manager, StringItem *item)
{
	const int MORE_SPACE_FOR_DECOMPRESS = 24;
	unsigned int compress_size, original_size;
	U8 *compress_data = NULL;
	U8 *original_data = NULL;
	BOOL success = FALSE;

	compress_size = item->compress_size;
	original_size = item->original_size;
	compress_data = (U8 *)gui_malloc(ALIGN_MEM_WITH_UNIT(compress_size, 4) + MORE_SPACE_FOR_DECOMPRESS);
	original_data = (U8 *)gui_malloc(ALIGN_MEM_WITH_UNIT(original_size, 4) + MORE_SPACE_FOR_DECOMPRESS);
	if (compress_data == NULL || original_data == NULL)
		goto EXIT;

	if (manager->read(manager->handle, item->offset, compress_data, compress_size) != compress_size)
		goto EXIT;

	if (get_string_check_data(compress_data, compress_size) != item->check_data)
		goto EXIT;

	if (uncompress_data(original_data, &original_size, compress_data, compress_size) != 0)
		goto EXIT;

	success = TRUE;

EXIT:
	if (compress_data != NULL)
		gui_free(compress_data);

	if (success)
		return original_data;

	if (original_data != NULL)
		gui_free(original_data);

	return NULL;
}

static int get_string_content_utf8_bytes_num(const char *string_data, U32 data_length, U32 encoding)
{
	int data_pos, prev_data_pos;
	int bytes_num = 0;

	str_max_strings = 0;
	prev_data_pos = 0;

	for (data_pos = 1; data_pos < data_length; data_pos++)
	{
		if (string_data[data_pos] == 0)
		{
			bytes_num += convert_to_utf8_bytes_num(string_data + prev_data_pos, data_pos - prev_data_pos, encoding);
			bytes_num++;	/* one more byte for '\0' */
			str_max_strings++;
			prev_data_pos = data_pos + 1;
		}
	}

	all_strings = (char **)malloc(str_max_strings * sizeof(char *));
	return bytes_num;
}

static BOOL set_string_content(int string_index, char *content)
{
	if (string_index < 0 || string_index >= str_max_strings)
		return FALSE;

	all_strings[string_index] = content;

	return TRUE;
}

static void set_string_content_with_data(const char *string_data, U32 data_length, U32 encoding)
{
	char *utf8_buf;
	U32 buf_size;
	int string_index = 0;
	int data_pos, prev_data_pos;
	int utf8_buf_remain;
	int string_size;
	int error_code;

	if (string_buffer_for_cur_language)
		gui_free(string_buffer_for_cur_language);

	all_strings = (char**)gui_malloc(sizeof(char*));
	buf_size = get_string_content_utf8_bytes_num((const char *)string_data, data_length, encoding);
	string_buffer_for_cur_language = (char *)gui_malloc(buf_size);
	if (is_id_language)
	{
		string_buffer_for_id_language = (char *)gui_malloc(buf_size);
	}
	if (string_buffer_for_cur_language == NULL)
		goto FINISH;

	utf8_buf = string_buffer_for_cur_language;
	if (is_id_language)
    {
        utf8_buf = string_buffer_for_id_language;
    }
	utf8_buf_remain = buf_size;
	prev_data_pos = 0;
	for (data_pos = 1; data_pos < data_length; data_pos++)
	{
		if (string_data[data_pos] != 0)
		{
			continue;
		}
		/* one string end */
		error_code = convert_to_utf8(string_data + prev_data_pos, data_pos - prev_data_pos, utf8_buf, utf8_buf_remain, encoding);
		if (error_code != 0)
		{
			gui_printf("convert language string to utf8 error, code: %d\n", error_code);
		}
		set_string_content(string_index, utf8_buf);
		string_index++;

		prev_data_pos = data_pos + 1;
		string_size = gui_strlen(utf8_buf) + 1;
		if (utf8_buf_remain < string_size)
		{
			break;
		}
		utf8_buf_remain -= string_size;
		utf8_buf += string_size;
	}
	if (is_id_language)
	{
		is_id_language = FALSE;
	}
FINISH:
	if (string_index != str_max_strings)
	{
		gui_printf("string number NOT match! ENUM: %d, ROM: %d.\n", str_max_strings, string_index);
	}
	/* fill with empty string if number of flash string less than stb */
	for (; string_index < str_max_strings; string_index++)
	{
		set_string_content(string_index, "");
	}
}

static int set_language_internal(StringListManager *manager, int lan_index)
{
	StringItem *string_item;
	U8 *string_data = NULL;

	if (lan_index < 0 || lan_index >= manager->item_num)
		return -1;

	string_item = manager->string_items + lan_index;
	string_data = get_string_item_data(manager, string_item);
	if (string_data == NULL)
		return -2;

	set_string_content_with_data((const char *)string_data, string_item->original_size, string_item->encoding);
	gui_free(string_data);

	return TRUE;
}

static int init_string_manager(StringListManager *manager, int min_ver, int max_ver)
{
	int item_info_size;
	int language_num = 0;
	int language;
	StringFileHeader header;
	StringItem *item;
	int i = 0;

	if (manager->read(manager->handle, 0, (U8 *)&header, sizeof(StringFileHeader)) != sizeof(StringFileHeader))
		return -1;

	if (strncmp(header.header_flag, "STRG", 4) != 0 || header.version < min_ver || header.version > max_ver)
	{
		gui_printf("unknown string data structure!\n");
		return -2;
	}

	item_info_size = header.item_num * sizeof(StringItem);
	if (item_info_size <= 0)
		return -3;

	manager->string_items = (StringItem *)gui_malloc(item_info_size);
	if (manager->string_items == NULL)
		return -4;

	manager->item_num = header.item_num;
	if (manager->read(manager->handle, sizeof(StringFileHeader), (U8 *)(manager->string_items), item_info_size) != item_info_size)
	{
		gui_free(manager->string_items);
		return -4;
	}

	for (i = 0; i < manager->item_num; i++)
	{
		item = manager->string_items + i;
		if (strcmp(item->language, "ttx") == 0)	/* it is teletext data, not a language */
		{
			continue;
		}
		if (item->compress_size <= 0 || item->original_size <= 0)
		{
			continue;
		}
		language = get_language_index_by_string_mark(item->language);
		if (!IsValidLanguage(language))
		{
			continue;
		}
		language_index[language_num++] = language;
	}
	for (i = 0; i < MAX_LANGUAGE; i++)
	{
		if (!IsValidLanguage(get_language_real_pos(i)))
		{
			language_index[language_num++] = (i | 0x100);
		}
	}
	if (language_num == 0)	/* no valid language in flash */
	{
		language_index[0] = 0;
	}
	return 0;
}

static StringListManager* string_manager_with_file(const char *filename, int min_ver, int max_ver)
{
	FILE *str_file;
	StringListManager *manager;

	str_file = fopen(filename, "rb");
	if (str_file == NULL)
		return NULL;

	manager = (StringListManager *)gui_malloc(sizeof(StringListManager));
	if (manager == NULL)
		return NULL;

	manager->handle = str_file;
	manager->read = read_string_file;
	manager->close_handle = close_string_file;

	if (init_string_manager(manager, min_ver, max_ver) != 0)
	{
		gui_printf("init string manager wrong \n");
		free(manager);
		fclose(str_file);
		return NULL;
	}

	return manager;
}

void set_id_language(int id_language)
{
	if (global_string_manager == NULL)
	{
		return;
	}
	is_id_language = TRUE;

	set_language_internal(global_string_manager, id_language);
}

void load_string_file(const char *filename)
{
	delete_string_manager(global_string_manager);
	global_string_manager = string_manager_with_file(filename, 0x0200, 0x0200);
}

void set_language(const char *language)
{
	int i;
	int language_index = 0;
	if (global_string_manager == NULL)
	{
		return;
	}
	language_index = get_language_real_pos(get_language_index_by_string_mark(language));
	for (i = 0; i < global_string_manager->item_num; i++)
	{
		if (strcmp(global_string_manager->string_items[i].language, "ttx") == 0
			|| strcmp(global_string_manager->string_items[i].language, "id") == 0)
		{
			language_index++;
		}
	}

	set_language_internal(global_string_manager, language_index);
}


//-------------------------------------------------------------------------------------------------
// logo - only for designer
//-------------------------------------------------------------------------------------------------
#define MAX_LOGO_NUM			 20

typedef struct _logo_data_
{
	unsigned char *logo_addr;
	unsigned int data_size;
} LogoData;

static LogoData global_logo_data[MAX_LOGO_NUM];


int set_logo_data(int index, unsigned char *data, unsigned int length)
{
	if(index >= MAX_LOGO_NUM || index < 0 || NULL == data)
	{
		return -1;
	}
	unsigned char *logo_addr = (unsigned char *)gui_malloc(length);
	if(NULL == logo_addr)
	{
		return -1;
	}
	gui_memcpy(logo_addr,data,length);

	if(global_logo_data[index].logo_addr)
	{
		gui_free(global_logo_data[index].logo_addr);
	}
	global_logo_data[index].logo_addr = logo_addr;
	global_logo_data[index].data_size = length;
	return 0;
}

