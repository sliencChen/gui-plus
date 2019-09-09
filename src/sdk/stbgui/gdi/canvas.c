/*
 * canvas.c
 *
 *  Created on: 2019-3-17
 *      Author: Brant
 */

#include <gdi/gui_interface.h>
#include <gdi/rect.h>
#include <gdi/color.h>
#include <gdi/canvas.h>


//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------
UICanvas main_canvas;


//-------------------------------------------------------------------------------------------------
// color operation
//-------------------------------------------------------------------------------------------------
#define ARGB1555_ARGB8888(x) ((x&0x8000) ? (0xff000000|(((x&0x7c00)<<9)|((x&0x3e0)<<6)|((x&0x1f)<<3))) : (((x&0x7c00)<<9)|((x&0x3e0)<<6)|((x&0x1f)<<3)))
#define ARGB8888_ARGB1555(x) (((x & 0x80000000) >> 16) | ((x & 0xf80000) >> 9) | ((x & 0xf800) >> 6) | ((x & 0xf8) >> 3))

#define ABS_SUBTRACT(x, y) 	((x)>=(y) ? ((x)-(y)):((y)-(x)))
#define CURCOLOR(a, f, b)   ((f)>(b) ? (b)+(ABS_SUBTRACT(f,b)*(a)>>8) : (b)-(ABS_SUBTRACT(f,b)*(a)>>8))


static inline U32 blend_color_argb8888(U32 bkg_color, U32 fore_color)
{
	U8 A1, R1, G1, B1;
	U8 A2, R2, G2, B2;
	U8 A, R, G, B;

	A1 = (bkg_color & 0xff000000) >> 24;
	R1 = (bkg_color & 0x00ff0000) >> 16;
	G1 = (bkg_color & 0x0000ff00) >> 8;
	B1 = (bkg_color & 0x000000ff);
	A2 = (fore_color & 0xff000000) >> 24;
	R2 = (fore_color & 0x00ff0000) >> 16;
	G2 = (fore_color & 0x0000ff00) >> 8;
	B2 = (fore_color & 0x000000ff);

	A = 0xff ^ (((0xff ^ A1) * (0xff ^ A2)) / 0xff);

	R = CURCOLOR(A2, R2, R1);
	G = CURCOLOR(A2, G2, G1);
	B = CURCOLOR(A2, B2, B1);

	return A << 24 | R << 16 | G <<8 | B;
}

static inline UIColor mix_color_argb8888(UIColor bkg_color, UIColor fore_color)
{
	U32 A1, R1, G1, B1;
	U32 A2, R2, G2, B2;
	U32 A, R, G, B;
	U32 uiTemp = 0;

	A1 = (bkg_color & 0xff000000) >> 24;
	R1 = (bkg_color & 0x00ff0000) >> 16;
	G1 = (bkg_color & 0x0000ff00) >> 8;
	B1 = (bkg_color & 0x000000ff);
	A2 = (fore_color & 0xff000000) >> 24;
	R2 = (fore_color & 0x00ff0000) >> 16;
	G2 = (fore_color & 0x0000ff00) >> 8;
	B2 = (fore_color & 0x000000ff);

	/* A = (U8)((1 - (1 - a1) * (1 - a2)) * 256); */	//a1 = A1 / 256, a2 = A2 / 256
	/* R = (U8)((float)(R1 * a1 * (1 - a2) + R2 * a2) / (float)(a1 + a2 - a1 * a2)); */
	A = A1 + A2 - ((A1 * A2) >> 8);
	if (A > 0xff)
	{
		A = 0xff;
	}
	if (0 == A)
	{
		return 0x00000000;
	}
	uiTemp = ((A2 << 8) / A);
	R = R1 + (((R2 - R1) * uiTemp) >> 8);
	G = G1 + (((G2 - G1) * uiTemp) >> 8);
	B = B1 + (((B2 - B1) * uiTemp) >> 8);

	return ((A & 0xff) << 24) | ((R & 0xff) << 16) | ((G & 0xff) << 8) | (B & 0xff);
}


//-------------------------------------------------------------------------------------------------
// canvas callback
//-------------------------------------------------------------------------------------------------
static U32 argb8888_color(struct ui_pixel_buf_t *self, UIColor color)
{
	return color;
}

static U32 argb8888_to_argb1555(struct ui_pixel_buf_t *self, UIColor color)
{
	return ARGB8888_ARGB1555(color);
}

static U32 get_pixel_32(struct ui_pixel_buf_t *self, int x, int y)
{
	U32 *addr = (U32 *)(self->addr);
	return addr[y * self->size.width + x];
}

static U32 get_pixel_16(struct ui_pixel_buf_t *self, int x, int y)
{
	U16 *addr = (U16 *)(self->addr);
	return addr[y * self->size.width + x];
}

static void set_pixel_32(struct ui_pixel_buf_t *self, int x, int y, U32 color)
{
	U32 *addr = (U32 *)(self->addr);
	addr[y * self->size.width + x] = color;
}

static void set_pixel_16(struct ui_pixel_buf_t *self, int x, int y, U32 color)
{
	U16 *addr = (U16 *)(self->addr);
	addr[y * self->size.width + x] = (U16)color;
}

#if 0
static void set_pixel_alpha_argb8888(struct ui_pixel_buf_t *self, int x, int y, U8 alpha)
{
	U32 *addr = (U32 *)(self->addr);
	U32 bkg_color = addr[y * self->size.width + x];
	addr[y * self->size.width + x] = (bkg_color & 0x00ffffff) | (alpha << 24);
}

static void set_pixel_alpha_argb1555(struct ui_pixel_buf_t *self, int x, int y, U8 alpha)
{
	U16 *addr = (U16 *)(self->addr);
	U16 bkg_color = addr[y * self->size.width + x];
	addr[y * self->size.width + x] = (bkg_color & 0x7fff) | ((alpha & 0x80) << 8);
}
#endif

static void blend_argb8888(struct ui_pixel_buf_t *self, int x, int y, U32 color)
{
	U32 *addr = (U32 *)(self->addr);
	U32 bkg_color = addr[y * self->size.width + x];
	addr[y * self->size.width + x] = blend_color_argb8888(bkg_color, color);
}

static void blend_argb1555_to_argb8888(struct ui_pixel_buf_t *self, int x, int y, U32 color)
{
	if (color & 0x8000)		// alpha is 1
	{
		U32 *addr = (U32 *)(self->addr);
		addr[y * self->size.width + x] = ARGB1555_ARGB8888(color);
	}
}

void* get_buf_addr_32(struct ui_pixel_buf_t *self, int x, int y)
{
	U32 *addr = (U32 *)(self->addr);
	return addr + y * self->size.width + x;
}

void* get_buf_addr_16(struct ui_pixel_buf_t *self, int x, int y)
{
	U16 *addr = (U16 *)(self->addr);
	return addr + y * self->size.width + x;
}

void set_buf_32(struct ui_pixel_buf_t *self, int x, int y, void *pixels, int pixel_num)
{
	U32 *addr = (U32 *)(self->addr);
	gui_memcpy(addr + y * self->size.width + x, pixels, pixel_num * 4);
}

void set_buf_16(struct ui_pixel_buf_t *self, int x, int y, void *pixels, int pixel_num)
{
	U16 *addr = (U16 *)(self->addr);
	gui_memcpy(addr + y * self->size.width + x, pixels, pixel_num * 2);
}

static void blend_argb8888_to_argb1555(struct ui_pixel_buf_t *self, int x, int y, U32 color)
{
	U16 *addr = (U16 *)(self->addr);
	U32 bkg_color = addr[y * self->size.width + x];
	U32 result = blend_color_argb8888(ARGB1555_ARGB8888(bkg_color), color);
	addr[y * self->size.width + x] = ARGB8888_ARGB1555(result);
}

static void blend_argb1555(struct ui_pixel_buf_t *self, int x, int y, U32 color)
{
	if (color & 0x8000)		// alpha is 1
	{
		U16 *addr = (U16 *)(self->addr);
		addr[y * self->size.width + x] = color;
	}
}

void mix_argb8888(struct ui_pixel_buf_t *self, int x, int y, U32 color)
{
	U32 *addr = (U32 *)(self->addr);
	U32 bkg_color = addr[y * self->size.width + x];
	addr[y * self->size.width + x] = mix_color_argb8888(bkg_color, color);
}

static void mix_argb8888_to_argb1555(struct ui_pixel_buf_t *self, int x, int y, U32 color)
{
	U16 *addr = (U16 *)(self->addr);
	U32 bkg_color = addr[y * self->size.width + x];
	U32 result = mix_color_argb8888(ARGB1555_ARGB8888(bkg_color), color);
	addr[y * self->size.width + x] = ARGB8888_ARGB1555(result);
}


static void set_pixel_buf_callback(struct ui_pixel_buf_t *pixel_buf)
{
	switch (pixel_buf->color_mode)
	{
		case COLOR_MODE_ARGB8888:
			pixel_buf->conv_color = argb8888_color;
			pixel_buf->get_pixel = get_pixel_32;
			pixel_buf->set_pixel = set_pixel_32;
//			pixel_buf->set_pixel_alpha = set_pixel_alpha_argb8888;
			pixel_buf->get_buf = get_buf_addr_32;
			pixel_buf->set_buf = set_buf_32;
			pixel_buf->blend_8888 = blend_argb8888;
			pixel_buf->blend_1555 = blend_argb1555_to_argb8888;
			pixel_buf->mix_8888 = mix_argb8888;
			break;

		case COLOR_MODE_ARGB1555:
		default:
			pixel_buf->conv_color = argb8888_to_argb1555;
			pixel_buf->get_pixel = get_pixel_16;
			pixel_buf->set_pixel = set_pixel_16;
//			pixel_buf->set_pixel_alpha = set_pixel_alpha_argb1555;
			pixel_buf->get_buf = get_buf_addr_16;
			pixel_buf->set_buf = set_buf_16;
			pixel_buf->blend_8888 = blend_argb8888_to_argb1555;
			pixel_buf->blend_1555 = blend_argb1555;
			pixel_buf->mix_8888 = mix_argb8888_to_argb1555;
			break;
	}
}


//-------------------------------------------------------------------------------------------------
// interface - create/destroy
//-------------------------------------------------------------------------------------------------
void init_main_canvas(void *buf, UISize size, UIColorMode mode)
{
	create_pixel_buf(&main_canvas, buf, size, mode);
}

BOOL create_pixel_buf(UIPixelBuf *pixel_buf, void *buf, UISize size, UIColorMode mode)
{
	if (!pixel_buf || is_empty_size(size))
	{
		return FALSE;
	}

	int pixel_size = get_color_pixel_size(mode);

	gui_memset(pixel_buf, 0, sizeof(UIPixelBuf));
	if (buf == NULL)	/* create a buf if it doesn't exist */
	{
		buf = gui_malloc(size.width * size.height * pixel_size);
	}
	if (buf == NULL)
		return FALSE;

	pixel_buf->addr = buf;
	pixel_buf->size = size;
	pixel_buf->color_mode = mode;
	pixel_buf->pixel_size = pixel_size;

	set_pixel_buf_callback(pixel_buf);

	return TRUE;
}

void destroy_pixel_buf(UIPixelBuf *pixel_buf)
{
	if (pixel_buf && pixel_buf->addr)
	{
		gui_free(pixel_buf->addr);
		gui_memset(pixel_buf, 0, sizeof(UIPixelBuf));
	}
}


//-------------------------------------------------------------------------------------------------
// interface - get/set
//-------------------------------------------------------------------------------------------------
UICanvas* get_main_canvas(void)
{
	return &main_canvas;
}

U8 get_canvas_pixel_size(UICanvas *canvas)
{
	if (canvas)
		return canvas->pixel_size;

	return 2;
}

U8 get_canvas_color_mode(UICanvas *canvas)
{
	if (canvas)
		return canvas->color_mode;

	return COLOR_MODE_ARGB1555;
}

int get_canvas_width(UICanvas *canvas)
{
	if (canvas)
		return canvas->size.width;

	return 1280;
}

int get_canvas_height(UICanvas *canvas)
{
	if (canvas)
		return canvas->size.height;

	return 720;
}

U32 get_canvas_color(UICanvas *canvas, UIColor color)
{
	if (canvas && canvas->conv_color)
		return canvas->conv_color(canvas, color);

	return color;
}

U32  get_canvas_pixel(UICanvas *canvas, int x, int y)
{
	if (x < 0 || y < 0 || x >= canvas->size.width || y >= canvas->size.height)
		return 0;

	return canvas->get_pixel(canvas, x, y);
}

void set_canvas_pixel(UICanvas *canvas, int x, int y, U32 color)
{
	if (x < 0 || y < 0 || x >= canvas->size.width || y >= canvas->size.height)
		return;

	canvas->set_pixel(canvas, x, y, color);
}

#if 0
void set_canvas_pixel_alpha(UICanvas *canvas, int x, int y, U8 alpha)
{
	if (x < 0 || y < 0 || x >= canvas->size.width || y >= canvas->size.height)
		return;

	canvas->set_pixel_alpha(canvas, x, y, alpha);
}
#endif

void* get_canvas_buf(UICanvas *canvas, int x, int y)
{
	if (x < 0 || y < 0 || x >= canvas->size.width || y >= canvas->size.height)
		return NULL;

	return canvas->get_buf(canvas, x, y);
}

void set_canvas_buf(UICanvas *canvas, int x, int y, void *pixels, int pixel_num)
{
	if (x < 0 || y < 0 || x >= canvas->size.width || y >= canvas->size.height)
		return;

	return canvas->set_buf(canvas, x, y, pixels, pixel_num);
}

void blend_canvas_8888(UICanvas *canvas, int x, int y, U32 color)
{
	if (x < 0 || y < 0 || x >= canvas->size.width || y >= canvas->size.height)
		return;

	canvas->blend_8888(canvas, x, y, color);
}

void blend_canvas_1555(UICanvas *canvas, int x, int y, U32 color)
{
	if (x < 0 || y < 0 || x >= canvas->size.width || y >= canvas->size.height)
		return;

	canvas->blend_1555(canvas, x, y, color);
}

void mix_canvas_8888(UICanvas *canvas, int x, int y, U32 color)
{
	if (x < 0 || y < 0 || x >= canvas->size.width || y >= canvas->size.height)
		return;

	canvas->mix_8888(canvas, x, y, color);
}







//#define RGB24_RGB1555(A,R,G,B) (A | (((R)>>3)<<10)|(((G)>>3)<<5)|((B)>>3)) //RGB888 to ARGB1555
//#define ARGB32_ARGB1555(A,R,G,B) ((((A) & 0x80) << 8) | (((R)>>3)<<10) | (((G)>>3)<<5) | ((B)>>3))
//#define RGB24_RGB565(R,G,B) ((((R)>>3)<<11)|(((G)>>2)<<5)|((B)>>3)) //RGB888 to ARGB565
//
//
//
//
//static inline U32 argb1555_to_argb8888(U32 color_1555)
//{
//	U32 color_8888;
//
//	color_8888 = (color_1555 & 0x8000) ? 0xff000000 : 0x00000000;
//	color_8888 |= ((color_1555 & 0x7c00) << 9) | ((color_1555 & 0x03e0) << 6) | ((color_1555 & 0x001f) << 3);
//
//	return color_8888;
//}
//
//static inline U32 rgb565_to_argb8888(U32 color_565)
//{
//	U32 color_8888;
//
//	color_8888 = color_565 == 0 ? 0x00000000 : 0xff000000;
//	color_8888 |= ((color_565 & 0xf800) << 8) | ((color_565 & 0x07e0) << 5) | ((color_565 & 0x001f) << 3);
//
//	return color_8888;
//}
//
//static inline U32 argb8888_to_rgb565(U32 color_8888)
//{
//	if ((color_8888 & 0x80000000) == 0)
//	{
//		return 0;	/* transparent */
//	}
//	return RGB24_RGB565(((color_8888&0xFF0000)>>16), ((color_8888&0xFF00)>>8), (color_8888&0xFF));
//}
//
//
//static inline void CopyBufferTo32bitCanvas(void* pDest, void* pSrc, int iDestIndex, int iSrcIndex, int iLen)
//{
//	U32* pDestBuff = (U32*)pDest;
//	U32* pSrcBuff = (U32*)pSrc;
//	memcpy(pDestBuff + iDestIndex , pSrcBuff + iSrcIndex, iLen * GetPixelSize());
//}
//
//static inline void CopyBufferTo16bitCanvas(void* pDest, void* pSrc, int iDestIndex, int iSrcIndex, int iLen)
//{
//	U16* pDestBuff = (U16*)pDest;
//	U16* pSrcBuff = (U16*)pSrc;
//	memcpy(pDestBuff + iDestIndex , pSrcBuff + iSrcIndex, iLen * GetPixelSize());
//}
//
//static inline U32 Get32bitCanvasBuffer(void* pAddr, int iIndex)
//{
//	U32* pBuffer = (U32*)pAddr;
//	return pBuffer[iIndex];
//}
//
//static inline U32 Get16bitCanvasBuffer(void* pAddr, int iIndex)
//{
//	U16* pBuffer = (U16*)pAddr;
//	return (U32)pBuffer[iIndex];
//}
//
//static inline void Set32bitCanvasBuffer(void* pAddr, int iIndex, U32 uiValue)
//{
//	U32* pBuffer = (U32*)pAddr;
//	pBuffer[iIndex] = uiValue;
//}
//
//static inline void Set16bitCanvasBuffer(void* pAddr, int iIndex, U32 uiValue)
//{
//	U16* pBuffer = (U16*)pAddr;
//	pBuffer[iIndex] = (U16)(uiValue & 0xffff);
//}
//
//static inline UIColor GetCanvasColorARGB8888(U32 uiColor)
//{
//	return uiColor;
//}
//
//static inline UIColor GetCanvasColorRGB565(U32 uiColor)
//{
//	return RGB24_RGB565(((uiColor&0xFF0000)>>16), ((uiColor&0xFF00)>>8), (uiColor&0xFF));
//}
//
//static inline UIColor GetCanvasColorARGB1555(U32 uiColor)
//{
//	U32 ucAlpha;
//	if((uiColor & 0xFF000000) == 0)
//		ucAlpha = 0;
//	else
//		ucAlpha = 0x8000;
//	return RGB24_RGB1555(ucAlpha ,((uiColor&0xFF0000)>>16), ((uiColor&0xFF00)>>8), (uiColor&0xFF));
//}
//
//U8 GetPixelSize(void)
//{
//	return _CanvasInfo.uPixelSize;
//}
//
//UIColorMode GetRGBMode(void)
//{
//	return _CanvasInfo.RGBMode;
//}
//
//U32 GetCanvasAt(void* Addr, U32 Index)
//{
//	return _CanvasInfo.GetBufferAt(Addr, Index);
//}
//
//void SetCanvasAt(void* Addr, U32 Index, U32 Color)
//{
//	_CanvasInfo.SetBufferAt(Addr, Index, Color);
//}
//
//void CopyBuffToCanvas(void* pDest, void* pSrc, int iDestIndex, int iSrcIndex, int iLen)
//{
//	_CanvasInfo.CopyBuffToCanvas(pDest, pSrc, iDestIndex, iSrcIndex, iLen);
//}
//
//U32 GetCanvasColor(U32 uiColor)
//{
//	return _CanvasInfo.GetCanvasColor(uiColor);
//}
//
//UIColor GetCanvasBlack(void)
//{
//	return _CanvasInfo.CanvasBlackColor;
//}
//
//static UIColor BlendBackColor1555(UIColor uiBackColor, UIColor uiFrontColor)
//{
//	U32 uR, uG, uB;
//	UIColor Color;
//
//	uR = (((uiBackColor & 0x00007C00) + (uiFrontColor & 0x00007C00)) >> 1) & 0x00007C00;
//	uG = (((uiBackColor & 0x000003e0) + (uiFrontColor & 0x000003e0)) >> 1) & 0x000003e0;
//	uB = (((uiBackColor & 0x0000001f) + (uiFrontColor & 0x0000001f)) >> 1) & 0x0000001f;
//	Color = 0x8000 + uR + uG + uB;
//
//	return Color;
//}
//
//static UIColor BlendBackColor565(UIColor uiBackColor, UIColor uiFrontColor)
//{
//	U32 uR, uG, uB;
//	UIColor Color;
//
//	uR = (((uiBackColor & 0x0000F800) + (uiFrontColor & 0x0000F800)) >> 1) & 0x0000F800;
//	uG = (((uiBackColor & 0x000007e0) + (uiFrontColor & 0x000007e0)) >> 1) & 0x000007e0;
//	uB = (((uiBackColor & 0x0000001f) + (uiFrontColor & 0x0000001f)) >> 1) & 0x0000001f;
//	Color = uR + uG + uB;
//
//	return Color;
//}
//
//
//
//static void BlendFontAt32(void *Addr, int iIndex, U32 uiColor)
//{
//	U32* pBuffer = (U32*)Addr;
//
//	pBuffer[iIndex] = blend_color_argb8888(pBuffer[iIndex], uiColor);
//}
//
//static void BlendFontAt565(void *Addr, int iIndex, U32 uiColor)
//{
//	U16* pBuffer = (U16*)Addr;
//	U32 uiBackColor32;
//	U32 uiBlendColor;
//
//	/* RGB565 to ARGB8888 */
//	uiBackColor32 = rgb565_to_argb8888(pBuffer[iIndex]);
//	/* blend */
//	uiBlendColor = blend_color_argb8888(uiBackColor32, uiColor);
//	/* ARGB8888 to RGB565 */
//	pBuffer[iIndex] = (U16)argb8888_to_rgb565(uiBlendColor);
//}
//
//static void BlendFontAt1555(void *Addr, int iIndex, U32 uiColor)
//{
//	U16* pBuffer = (U16*)Addr;
//	U32 uiBackColor32;
//	U32 uiBlendColor;
//
//	/* ARGB1555 to ARGB8888 */
//	uiBackColor32 = argb1555_to_argb8888(pBuffer[iIndex]);
//	/* blend */
//	uiBlendColor = blend_color_argb8888(uiBackColor32, uiColor);
//	/* ARGB8888 to ARGB1555 */
//	pBuffer[iIndex] = ARGB32_ARGB1555(uiBlendColor >> 24 ,((uiBlendColor&0xFF0000)>>16), ((uiBlendColor&0xFF00)>>8), (uiBlendColor&0xFF));
//}
//
//UIColor MixBackColor32(UIColor uiBackColor, UIColor uiFrontColor)
//{
//	U32 A1, R1, G1, B1;
//	U32 A2, R2, G2, B2;
//	U32 A, R, G, B;
//	U32 uiTemp = 0;
//
//	A1 = (uiBackColor & 0xff000000) >> 24;
//	R1 = (uiBackColor & 0x00ff0000) >> 16;
//	G1 = (uiBackColor & 0x0000ff00) >> 8;
//	B1 = (uiBackColor & 0x000000ff);
//	A2 = (uiFrontColor & 0xff000000) >> 24;
//	R2 = (uiFrontColor & 0x00ff0000) >> 16;
//	G2 = (uiFrontColor & 0x0000ff00) >> 8;
//	B2 = (uiFrontColor & 0x000000ff);
//
//	/* A = (U8)((1 - (1 - a1) * (1 - a2)) * 256); */	//a1 = A1 / 256, a2 = A2 / 256
//	/* R = (U8)((float)(R1 * a1 * (1 - a2) + R2 * a2) / (float)(a1 + a2 - a1 * a2)); */
//	A = A1 + A2 - ((A1 * A2) >> 8);
//	if (A > 0xff)
//	{
//		A = 0xff;
//	}
//	if (0 == A)
//	{
//		return 0x00000000;
//	}
//	uiTemp = ((A2 << 8) / A);
//	R = R1 + (((R2 - R1) * uiTemp) >> 8);
//	G = G1 + (((G2 - G1) * uiTemp) >> 8);
//	B = B1 + (((B2 - B1) * uiTemp) >> 8);
//
//	return ((A & 0xff) << 24) | ((R & 0xff) << 16) | ((G & 0xff) << 8) | (B & 0xff);
//}
//
//UIColor BlendBackColor(UIColor uiBackColor, UIColor uiFrontColor)
//{
//	return _CanvasInfo.BlendBackColor(uiBackColor, uiFrontColor);
//}
//
//
//static void BlendBmpAt565(U8 *pucDest, int iDestIndex, U8 *pucSrc, int iSrcIndex)
//{
//	U16 color = GET_CANVAS_AT(pucSrc, iSrcIndex);
//
//	if (color)
//	{
//		SET_CANVAS_AT(pucDest, iDestIndex, color);
//	}
//}
//
//static void blend_32bit_bmp_at_565(U8 *canvas, int canvas_index, U8 *bmp, int bmp_index)
//{
//	U16 *dst = (U16 *)canvas;
//	U32 *src = (U32 *)bmp;
//	U32 back_color;
//
//	back_color = rgb565_to_argb8888(dst[canvas_index]);
//	dst[canvas_index] = (U16)argb8888_to_rgb565(blend_color_argb8888(back_color, src[bmp_index]));
//}
//
//
//static void BlendBmpAt1555(U8 *pucDest, int iDestIndex, U8 *pucSrc, int iSrcIndex)
//{
//	U16 color = GetCanvasAt(pucSrc, iSrcIndex);
//
//	if (color & 0x8000)
//	{
//		SET_CANVAS_AT(pucDest, iDestIndex, color);
//	}
//}
//
//static void blend_32bit_bmp_at_1555(U8 *canvas, int canvas_index, U8 *bmp, int bmp_index)
//{
//	U16 *dst = (U16 *)canvas;
//	U32 *src = (U32 *)bmp;
//	U32 back_color;
//	U32 blend_color;
//
//	/* ARGB1555 to ARGB8888 */
//	back_color = argb1555_to_argb8888(dst[canvas_index]);
//	/* blend */
//	blend_color = blend_color_argb8888(back_color, src[bmp_index]);
//	/* ARGB8888 to ARGB1555 */
//	dst[canvas_index] = ARGB32_ARGB1555(blend_color >> 24 ,((blend_color&0xFF0000)>>16), ((blend_color&0xFF00)>>8), (blend_color&0xFF));
//}
//
//
//static void BlendBmpAt32(U8 *pucDest, int iDestIndex, U8 *pucSrc, int iSrcIndex)
//{
//	U32 uiBackColor = GetCanvasAt(pucDest, iDestIndex);
//	U32 uiFrontColor = GetCanvasAt(pucSrc, iSrcIndex);
//	SET_CANVAS_AT(pucDest, iDestIndex, blend_color_argb8888(uiBackColor, uiFrontColor));
//}
//
//#if (PLATFORM_SUPPORT == MSTAR_PLATFORM)
//static void blend_rgb565_bmp_at_32(U8 *canvas, int canvas_index, U8 *bmp, int bmp_index)
//{
//	U32 *dst = (U32 *)canvas;
//	U16 *src = (U16 *)bmp;
//	U32 back_color;
//
//	back_color = dst[canvas_index];
//	dst[canvas_index] = blend_color_argb8888(back_color, rgb565_to_argb8888(src[bmp_index]));
//}
//#endif
//
//static void blend_argb1555_bmp_at_32(U8 *canvas, int canvas_index, U8 *bmp, int bmp_index)
//{
//	U32 *dst = (U32 *)canvas;
//	U16 *src = (U16 *)bmp;
//	U32 back_color;
//
//	back_color = dst[canvas_index];
//	dst[canvas_index] = blend_color_argb8888(back_color, argb1555_to_argb8888(src[bmp_index]));
//}
//
//
//static void MixBmpAt32(U8 *pucDest, int iDestIndex, U8 *pucSrc, int iSrcIndex)
//{
//	U32 uiBackColor = GetCanvasAt(pucDest, iDestIndex);
//	U32 uiFrontColor = GetCanvasAt(pucSrc, iSrcIndex);
//	SET_CANVAS_AT(pucDest, iDestIndex, MixBackColor32(uiBackColor, uiFrontColor));
//}
//
//void BlendBmpAt(U8 *pucDest, int iDestIndex, U8 *pucSrc, int iSrcIndex)
//{
//	_CanvasInfo.BlendBmpAt(pucDest, iDestIndex, pucSrc, iSrcIndex);
//}
//
//void blend_32bit_bmp_at(U8 *dst, int dst_index, U8 *src, int src_index)
//{
//	_CanvasInfo.blend_32bit_bmp_at(dst, dst_index, src, src_index);
//}
//
//void blend_16bit_bmp_at(U8 *dst, int dst_index, U8 *src, int src_index)
//{
//	_CanvasInfo.blend_16bit_bmp_at(dst, dst_index, src, src_index);
//}
//
//void MixBmpAt(U8 *pucDest, int iDestIndex, U8 *pucSrc, int iSrcIndex)
//{
//	_CanvasInfo.MixBmpAt(pucDest, iDestIndex, pucSrc, iSrcIndex);
//}
//
//void BlendFontAt(void* Addr, int iIndex, U32 uiColor)
//{
//	_CanvasInfo.BlendFontAt(Addr, iIndex, uiColor);
//}
//
//static int InitCanvasInfo(gsCanvasInfo *CanvasInfo, UIColorMode RGBMode)
//{
//	if (CanvasInfo == NULL)
//		return -1;
//	switch(RGBMode)
//	{
//		case COLOR_MODE_ARGB8888:
//			CanvasInfo->GetBufferAt = Get32bitCanvasBuffer;
//			CanvasInfo->SetBufferAt = Set32bitCanvasBuffer;
//			CanvasInfo->CopyBuffToCanvas = CopyBufferTo32bitCanvas;
//			CanvasInfo->GetCanvasColor = GetCanvasColorARGB8888;
//			CanvasInfo->uPixelSize = 4;
//			CanvasInfo->CanvasBlackColor = 0xff000000;
//			CanvasInfo->BlendBmpAt = BlendBmpAt32;
//			CanvasInfo->blend_32bit_bmp_at = BlendBmpAt32;
//#if (PLATFORM_SUPPORT == MSTAR_PLATFORM)
//			CanvasInfo->blend_16bit_bmp_at = blend_rgb565_bmp_at_32;
//#else
//			CanvasInfo->blend_16bit_bmp_at = blend_argb1555_bmp_at_32;
//#endif
//			CanvasInfo->MixBmpAt = MixBmpAt32;
//			CanvasInfo->BlendBackColor = blend_color_argb8888;
//			CanvasInfo->BlendFontAt = BlendFontAt32;
//			break;
//		case COLOR_MODE_RGB565:
//			CanvasInfo->GetBufferAt = Get16bitCanvasBuffer;
//			CanvasInfo->SetBufferAt = Set16bitCanvasBuffer;
//			CanvasInfo->CopyBuffToCanvas = CopyBufferTo16bitCanvas;
//			CanvasInfo->GetCanvasColor = GetCanvasColorRGB565;
//			CanvasInfo->uPixelSize = 2;
//			CanvasInfo->CanvasBlackColor = 0x0821;
//			CanvasInfo->BlendBmpAt = BlendBmpAt565;
//			CanvasInfo->blend_32bit_bmp_at = blend_32bit_bmp_at_565;
//			CanvasInfo->blend_16bit_bmp_at = BlendBmpAt565;
//			CanvasInfo->MixBmpAt = BlendBmpAt565;
//			CanvasInfo->BlendBackColor = BlendBackColor565;
//			CanvasInfo->BlendFontAt = BlendFontAt565;
//			break;
//		case COLOR_MODE_ARGB1555:
//			CanvasInfo->GetBufferAt = Get16bitCanvasBuffer;
//			CanvasInfo->SetBufferAt = Set16bitCanvasBuffer;
//			CanvasInfo->CopyBuffToCanvas = CopyBufferTo16bitCanvas;
//			CanvasInfo->GetCanvasColor = GetCanvasColorARGB1555;
//			CanvasInfo->uPixelSize = 2;
//			CanvasInfo->CanvasBlackColor = 0x8000;
//			CanvasInfo->BlendBmpAt = BlendBmpAt1555;
//			CanvasInfo->blend_32bit_bmp_at = blend_32bit_bmp_at_1555;
//			CanvasInfo->blend_16bit_bmp_at = BlendBmpAt1555;
//			CanvasInfo->MixBmpAt = BlendBmpAt1555;
//			CanvasInfo->BlendBackColor = BlendBackColor1555;
//			CanvasInfo->BlendFontAt = BlendFontAt1555;
//			break;
//		default:
//			return -2;
//	}
//	return 0;
//}
