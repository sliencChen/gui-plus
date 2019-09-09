/*
 * uirender.cpp
 *
 *  Created on: 2019-3-17
 *      Author: Brant
 */

#include <string.h>
#include <wx/wx.h>
#include <gui.h>
#include "uirender.h"

//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------
unsigned char *canvasBuf = NULL;


//-------------------------------------------------------------------------------------------------
// window convertion
//-------------------------------------------------------------------------------------------------
static void ConvertToGuiWindowRecursively(uiWindow *window, UIWindow *parent, UIWindow **root)
{
	if (window == nullptr) return;

	if (window->GetClassName() == "UIEditItem")
		return;

	if (window->GetWindowType() == WINDOW_TYPE_SIZER)
    {
        /* this is sizer */
        window->stb_sizer = window->guiSizer();
        window->stb_window = parent;
    }
    else
    {
        window->stb_window = window->guiWindow();
        if (window->stb_window == nullptr) return;

        /* this is root window, need save root variable */
        if (parent == nullptr)
        {
            *root = window->stb_window;
        }
        else
        {
            if (window->m_Parent->GetWindowType() != WINDOW_TYPE_SIZER)
            {
                add_sub_window(parent, window->stb_window);
            }
        }
    }

	uiWindow *childs = window->m_Childs;
	while (childs)
	{
		ConvertToGuiWindowRecursively(childs, window->stb_window, root);
		childs = childs->m_Next;
	}
}

static UIWindow* GuiWindow(uiWindow *window)
{
	if (window == nullptr) return nullptr;

	UIWindow *guiroot = NULL;
	ConvertToGuiWindowRecursively(window, NULL, &guiroot);

	return guiroot;
}


//-------------------------------------------------------------------------------------------------
// image stretch
//-------------------------------------------------------------------------------------------------
//ARGB8888
static void bilinear_calculate_32bit(unsigned int* pixels, U8 u, U8 v, unsigned int* result)
{
	U32 color, a_g, r_b;
	U8 pm3 = (u * v) >> 8;
	U8 pm2 = u - pm3;
	U8 pm1 = v - pm3;
	short pm0 = 256 - pm1 - pm2 - pm3;

	color = (U32)pixels[0];
	a_g = ((color & 0xFF00FF00) >> 8) * pm0;
	r_b = (color & 0x00FF00FF) * pm0;
	color = (U32)pixels[1];
	a_g += ((color & 0xFF00FF00) >> 8) * pm2;
	r_b += (color & 0x00FF00FF) * pm2;
	color = (U32)pixels[2];
	a_g += ((color & 0xFF00FF00) >> 8) * pm1;
	r_b += (color & 0x00FF00FF) * pm1;
	color = (U32)pixels[3];
	a_g += ((color & 0xFF00FF00) >> 8) * pm3;
	r_b += (color & 0x00FF00FF) * pm3;

	*(U32*)(result) = (a_g & 0xFF00FF00) | ((r_b & 0xFF00FF00) >> 8);
}

static inline U32 get_bilinear_pixel_x(U32 *buffer, int width, S16 x)
{
	return x < 0 ? 0 : (x >= width ? (width - 1) : x);
}

static inline U32 get_bilinear_pixel_y(U32 *buffer, int height, S16 y)
{
	return y < 0 ? 0 : (y >= height ? (height - 1) : y);
}

static U32 get_bitmap_pixel(U32 *buffer, int width, int x, int y)
{
	return buffer[y * width + x];
}

static void set_bitmap_pixel(U32 *buffer, int width, int x, int y, U32 color)
{
	buffer[y * width + x] = color;
}

static void bilinear_border_pixels(U32 *buffer, int width, int height, S16 x, S16 y, U32 *pixels)
{
	pixels[0] = get_bitmap_pixel(buffer, width, get_bilinear_pixel_x(buffer, width, x), get_bilinear_pixel_y(buffer, height, y));
	pixels[1] = get_bitmap_pixel(buffer, width, get_bilinear_pixel_x(buffer, width, x + 1), get_bilinear_pixel_y(buffer, height, y));
	pixels[2] = get_bitmap_pixel(buffer, width, get_bilinear_pixel_x(buffer, width, x), get_bilinear_pixel_y(buffer, height, y + 1));
	pixels[3] = get_bitmap_pixel(buffer, width, get_bilinear_pixel_x(buffer, width, x + 1), get_bilinear_pixel_y(buffer, height, y + 1));
}

static void StretchBitmap32(unsigned int *buffer, int width, int height)
{
	int m_Width = main_canvas_width();
	int m_Height = main_canvas_height();
	unsigned int *srcBuffer = (unsigned int *)canvasBuf;
	/* perfect! */
	if (width == m_Width && height == m_Height)
	{
		memcpy(buffer, srcBuffer, width * height * 4);
		return;
	}

#if 0
	int x, y, src_x, src_y = 0;
	unsigned int x_ratio, y_ratio;

	x_ratio = (m_Width << 16) / width + 1;
	y_ratio = (m_Height << 16) / height + 1;
	for (y = 0; y < height; y++)
	{
		src_x = 0;
		for (x = 0; x < width; x++)
		{
			buffer[y * width + x] = srcBuffer[(src_y >> 16) * m_Width + (src_x >> 16)];
			src_x += x_ratio;
		}
		src_y += y_ratio;
	}
#else
	/*Formula:	Sx = (Dx + 0.5)*SW/DW - 0.5
	 * 			Sy = (Dy + 0.5)*SH/DH - 0.5
	 * 			u=Sx-floor(Sx); v=Sy-floor(Sy);
	 * 			pm0=(1-u)*(1-v);
	 * 			pm1=v*(1-u);
	 * 			pm2=u*(1-v);
	 * 			pm3=u*v;
	 * 			DstColor = Color0*pm0 + Color1*pm1 + Color2*pm2 + Color3*pm3;
	 */
	long x_ratio = (m_Width << 16) / width + 1;
	long y_ratio = (m_Height << 16) / height + 1;
	const long x_start = (x_ratio >> 1) - (1 << 15);  //Sx0 = (Dx0 + 0.5)*SW/DW - 0.5, Dx0 = 0;
	const long y_start = (y_ratio >> 1) - (1 << 15);

	long border_left = -x_start/x_ratio + 1; //Sx0+Dx*x_ratio >= 0 --> Dx >= -Sx0/x_ratio
	long border_top = -y_start/y_ratio + 1;
	long border_right = (((m_Width - 1) << 16) - x_start) / x_ratio + 1; //Sx0+Dx*x_ratio<SW-1 --> Dx < (SW-1-Sx0)/x_ratio
	long border_bottom = (((m_Height - 1) << 16) - y_start) / y_ratio + 1;

	U8 v;
	int x, y;
	U32 color;
	U32 pixels[4];
	long src_x, real_src_x, real_src_y;
	long src_y = y_start;
	long /*src_curr_row_offset,*/ /*src_next_row_offset,*/ dst_row_offset = 0;
//	BILINEAR_CALCULATE bilinear_calculate = NULL;
//	int color_mode;

	if (border_top >= height) border_top = height;
	if (border_left >= width) border_left = width;
	if (border_bottom < border_top) border_bottom = border_top;
	if (border_right < border_left) border_right = border_left;
//	if (get_bmp_version() <= 1)
//	{
//		color_mode = GetRGBMode();
//	}
//	else
//	{
//		color_mode = src->color_mode;
//	}
//	if (color_mode == gsARGB8888)
//	{
//		bilinear_calculate = bilinear_calculate_32bit;
//	}
//	else
//	{
//		bilinear_calculate = bilinear_calculate_16bit;
//	}

	//top border
	for (y = 0; y < border_top; ++y)
	{
		src_x = x_start;
		v = (src_y & 0xFFFF) >> 8;
		real_src_y = src_y >> 16;

		for (x = 0; x < width; ++x)
		{
			bilinear_border_pixels(srcBuffer, m_Width, m_Height, src_x >> 16, real_src_y, pixels);;
			bilinear_calculate_32bit(pixels, (src_x & 0xFFFF) >> 8, v, &color);
			set_bitmap_pixel(buffer, width, x, y, color);
			src_x += x_ratio;
		}

		src_y += y_ratio;
		dst_row_offset += width;
	}

	for (y = border_top; y < border_bottom; ++y)
	{
		src_x = x_start;
		v = (src_y & 0xFFFF) >> 8;
		real_src_y = src_y >> 16;
//		src_curr_row_offset = real_src_y * m_Width;
//		src_next_row_offset = src_curr_row_offset + m_Width;

		//left border
		for (x = 0; x < border_left; ++x)
		{
			bilinear_border_pixels(srcBuffer, m_Width, m_Height, src_x >> 16, real_src_y, pixels);
			bilinear_calculate_32bit(pixels, (src_x & 0xFFFF) >> 8, v, &color);
			set_bitmap_pixel(buffer, width, x, y, color);
			src_x += x_ratio;
		}

		for (x = border_left; x < border_right; ++x)
		{
			real_src_x = src_x >> 16;
			pixels[0] = get_bitmap_pixel(srcBuffer, m_Width, real_src_x, real_src_y);
			pixels[1] = get_bitmap_pixel(srcBuffer, m_Width, real_src_x + 1, real_src_y);
			pixels[2] = get_bitmap_pixel(srcBuffer, m_Width, real_src_x, real_src_y + 1);
			pixels[3] = get_bitmap_pixel(srcBuffer, m_Width, real_src_x + 1, real_src_y + 1);
			bilinear_calculate_32bit(pixels, (src_x & 0xFFFF) >> 8, v, &color);
			set_bitmap_pixel(buffer, width, x, y, color);
			src_x += x_ratio;
		}

		//right border
		for (x = border_right; x < width; ++x)
		{
			bilinear_border_pixels(srcBuffer, m_Width, m_Height, src_x >> 16, real_src_y, pixels);
			bilinear_calculate_32bit(pixels, (src_x & 0xFFFF) >> 8, v, &color);
			set_bitmap_pixel(buffer, width, x, y, color);
			src_x += x_ratio;
		}

		src_y += y_ratio;
		dst_row_offset += width;
	}

	//bottom border
	for (y = border_bottom; y < height; ++y)
	{
		src_x = x_start;
		v = (src_y & 0xFFFF) >> 8;
		real_src_y = src_y >> 16;

		for (x = 0; x < width; ++x)
		{
			bilinear_border_pixels(srcBuffer, m_Width, m_Height, src_x >> 16, real_src_y, pixels);
			bilinear_calculate_32bit(pixels, (src_x & 0xFFFF) >> 8, v, &color);
			set_bitmap_pixel(buffer, width, x, y, color);
			src_x += x_ratio;
		}

		src_y += y_ratio;
		dst_row_offset += width;
	}
#endif
}

static void StretchBitmap1555(unsigned int *buffer, int width, int height)
{
	int m_Width = main_canvas_width();
	int m_Height = main_canvas_height();
	unsigned short *srcBuffer = (unsigned short *)canvasBuf;

	int x, y, src_x, src_y = 0;
	unsigned int x_ratio, y_ratio;
	unsigned short color1555, color32bit;

	x_ratio = (m_Width << 16) / width + 1;
	y_ratio = (m_Height << 16) / height + 1;
	for (y = 0; y < height; y++)
	{
		src_x = 0;
		for (x = 0; x < width; x++)
		{
			color1555 = srcBuffer[(src_y >> 16) * m_Width + (src_x >> 16)];
			color32bit = (color1555 & 0x8000) ? 0xff000000 : 0x00000000;
			color32bit |= ((color1555 & 0x7c00) << 9) | ((color1555 & 0x03e0) << 6) | ((color1555 & 0x001f) << 3);

			buffer[y * width + x] = color32bit;
			src_x += x_ratio;
		}
		src_y += y_ratio;
	}
}


//-------------------------------------------------------------------------------------------------
// render
//-------------------------------------------------------------------------------------------------
void uiRender::SetCanvasProperty(wxSize size, int colorMode)
{
	if (canvasBuf)
		delete canvasBuf;

	int pixelSize = get_color_pixel_size((UIColorMode)colorMode);
	canvasBuf = new unsigned char[size.GetWidth() * size.GetHeight() * pixelSize];

	init_main_canvas((void *)canvasBuf, ui_size(size.GetWidth(), size.GetHeight()), (UIColorMode)colorMode);
}

void uiRender::GetBitmap(uiWindow *window, unsigned int *buffer, int width, int height)
{
	memset(buffer, 0, width * height * sizeof(unsigned int));

	UIWindow *root = GuiWindow(window);
	if (root == NULL) return;

	//clear_main_canvas();
	flush_window(root);
	destroy_window(root);

	if (main_canvas_color_mode() == COLOR_MODE_ARGB8888)
	{
		return StretchBitmap32(buffer, width, height);
	}
	return StretchBitmap1555(buffer, width, height);
}
