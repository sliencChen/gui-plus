/*
 * canvas.h
 *
 *  Created on: 2019-3-17
 *      Author: Brant
 */

#ifndef _GUI_CANVAS_H_
#define _GUI_CANVAS_H_

#ifdef __cplusplus
extern "C"
{
#endif


//-------------------------------------------------------------------------------------------------
// create/destory
//-------------------------------------------------------------------------------------------------
void init_main_canvas(void *buf, UISize size, UIColorMode mode);
BOOL create_pixel_buf(UIPixelBuf *pixel_buf, void *buf, UISize size, UIColorMode mode);
void destroy_pixel_buf(UIPixelBuf *pixel_buf);


//-------------------------------------------------------------------------------------------------
// get/set
//-------------------------------------------------------------------------------------------------
UICanvas* get_main_canvas(void);

U8 get_canvas_pixel_size(UICanvas *canvas);
U8 get_canvas_color_mode(UICanvas *canvas);
int get_canvas_width(UICanvas *canvas);
int get_canvas_height(UICanvas *canvas);

U32 get_canvas_color(UICanvas *canvas, UIColor color);

U32  get_canvas_pixel(UICanvas *canvas, int x, int y);
void set_canvas_pixel(UICanvas *canvas, int x, int y, U32 color);

//void set_canvas_pixel_alpha(UICanvas *canvas, int x, int y, U8 alpha);

void* get_canvas_buf(UICanvas *canvas, int x, int y);
void  set_canvas_buf(UICanvas *canvas, int x, int y, void *pixels, int pixel_num);

void blend_canvas_8888(UICanvas *canvas, int x, int y, U32 color);
void blend_canvas_1555(UICanvas *canvas, int x, int y, U32 color);

void mix_canvas_8888(UICanvas *canvas, int x, int y, U32 color);


//-------------------------------------------------------------------------------------------------
// main canvas
//-------------------------------------------------------------------------------------------------
#define main_canvas_pixel_size()	(get_canvas_pixel_size(get_main_canvas()))
#define main_canvas_color_mode()	(get_canvas_color_mode(get_main_canvas()))
#define main_canvas_width()			(get_canvas_width(get_main_canvas()))
#define main_canvas_height()		(get_canvas_height(get_main_canvas()))


#ifdef __cplusplus
}
#endif

#endif	/* _GUI_CANVAS_H_ */
