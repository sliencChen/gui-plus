/*
 * image.c
 *
 *  Created on: 2018-7-6
 *      Author: Brant
 */

#include <gui_window.h>
#include <control/image.h>


//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// internal use
//-------------------------------------------------------------------------------------------------
UIImageData* new_image_private_data(void)
{
	UIImageData *img_data;

	img_data = (UIImageData *)gui_malloc(sizeof(UIImageData));
	if (img_data == NULL)
	{
		return NULL;
	}

	/* default parameter */
	gui_memset(img_data, 0, sizeof(UIImageData));
	img_data->auto_size = TRUE;
	img_data->mode = DRAW_BMP_MODE_NORMAL;

	return img_data;
}

static void delete_image_private_data(UIImageData *img_data)
{
	if (img_data)
	{
		gui_free(img_data);
	}
}

static int set_image_info_by_name(UIImageData *img_data, const char *name)
{
	img_data->is_ok = gui_get_bmp(name, &(img_data->bmp));

	return img_data->is_ok ? 0 : -1;
}


//-------------------------------------------------------------------------------------------------
// class event
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// window manager callback
//-------------------------------------------------------------------------------------------------
static BOOL image_on_draw(UIImage *self)
{
	UIPainter *painter = &(self->painter);
	UIRect rect = ui_rect(0, 0, self->rect.size.width, self->rect.size.height);
	UIImageData *img_data = (UIImageData *)self->private_data;
	DrawBmpParam param;

	if (!img_data->is_ok)
		return FALSE;

	param.bmp_info = img_data->bmp;
	param.mode = img_data->mode;
	param.mode_param = img_data->mode_param;

	draw_bitmap(painter, rect, &param);

	return TRUE;
}

static BOOL image_on_destroy(UIImage *self)
{
	delete_image_private_data(self->private_data);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UIImage* create_image(const char *id)
{
	UIWindow *image;

	image = create_control(id, 0, 0, 100, 100);
	if (image != NULL)
	{
		UIImageData *img_data = new_image_private_data();

		if (img_data == NULL)
		{
			gui_free(image);
			return NULL;
		}

		image->private_data = img_data;
		image->focus_stop = FALSE;
		image->class_name = IMAGE_CLASS_NAME;
		image->has_trans = FALSE;
		image->on_draw = image_on_draw;
		image->on_destroy = image_on_destroy;
	}
	return image;
}


int set_built_in_image(UIImage *image, const char *name)
{
	UIImageData *img_data;

	if (!image || !image->private_data)
	{
		return -1;
	}
	img_data = (UIImageData *)image->private_data;
	if (set_image_info_by_name(img_data, name) != 0)
	{
		return -1;
	}
	image->has_trans = img_data->bmp.has_trans;
	if (img_data->auto_size)
	{
		set_window_size(image, img_data->bmp.pixel_buf.size);
	}
	invalidate_window(image);

	return 0;
}


void stretch_image(UIImage *image, UISize size)
{
	UIImageData *img_data;

	if (!image || !image->private_data)
	{
		return;
	}
	img_data = (UIImageData *)image->private_data;
	img_data->mode = DRAW_BMP_MODE_STRETCH;
	img_data->mode_param.fast = FALSE;
	set_window_size(image, size);
	invalidate_window(image);
}


int set_image_auto_size(UIImage *image, BOOL auto_size)
{
    UIImageData *img_data;
    if (!image || !image->private_data)
	{
		return -1;
	}
	img_data = (UIImageData *)image->private_data;
    img_data->auto_size = auto_size;
    return 0;
}


int set_image_draw_mode(UIImage *image, DrawBmpMode mode)
{
    UIImageData *img_data;
    if (!image || !image->private_data)
	{
		return -1;
	}
	img_data = (UIImageData *)image->private_data;
	img_data->mode = mode;
	return 0;
}

