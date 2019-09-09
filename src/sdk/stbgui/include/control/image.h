/*
 * image.h
 *
 *  Created on: 2018-7-6
 *      Author: Brant
 */

#ifndef _GUI_IMAGE_H_
#define _GUI_IMAGE_H_

#ifdef __cplusplus
extern "C"
{
#endif


typedef UIWindow UIImage;
#define IMAGE_CLASS_NAME "image"


typedef struct _ui_image_data_
{
	UIBmp bmp;
	DrawBmpMode mode;
	DrawModeParam mode_param;
	U32 auto_size	:1;
	U32 is_ok		:1;
	U32 reserved	:30;
} UIImageData;


UIImage* create_image(const char *id);

int set_built_in_image(UIImage *image, const char *name);
void stretch_image(UIImage *image, UISize size);
int set_image_auto_size(UIImage *image, BOOL auto_size);
int set_image_draw_mode(UIImage *image, DrawBmpMode mode);


#ifdef __cplusplus
}
#endif

#endif /* _GUI_IMAGE_H_ */
