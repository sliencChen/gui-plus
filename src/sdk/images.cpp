/*
 * images.cpp
 *
 *  Created on: 2019-4-24
 *      Author: Brant
 */

#include <stdio.h>
#include <gdi/resource.h>
#include "sdk_common.h"
#include "images.h"


//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------
unsigned char *allBmpData = nullptr;


//-------------------------------------------------------------------------------------------------
// uiImage
//-------------------------------------------------------------------------------------------------
int Image::ProjectColorMode = 0;


Image::Image(const Image* image)
{
	m_FileName = image->GetFileName();
	m_Name = image->GetName();

	m_Width = image->GetWidth();
	m_Height = image->GetHeight();

	m_IsOK = image->IsOK();

	m_ColorMode = image->GetColorMode();
	m_HaveKeyColor = image->HaveKeyColor();
	m_KeyColor = image->GetKeyColor();

	m_SliceMode = image->GetSliceMode();

	m_SliceLeft = image->GetSliceLeft();
	m_SliceRight = image->GetSliceRight();
	m_SliceTop = image->GetSliceTop();
	m_SliceBottom = image->GetSliceBottom();

	m_FillMode = image->GetSliceFillMode();
	m_FillWidth = image->GetSliceFillWidth();
	m_FillHeight = image->GetSliceFillHeight();
}

Image::Image(const wxString& filename, const wxString& name)
{
	wxImage image(filename);

	m_IsOK = image.IsOk();

	if (m_IsOK)
	{
		m_Width = image.GetWidth();
		m_Height = image.GetHeight();
	}

	m_Name = name;
	m_FileName = filename;

	m_ColorMode = ColorMode_Auto;
	m_HaveKeyColor = false;
	m_KeyColor = 0;
}

int Image::GetWidth2(void)
{
	int width = m_Width;

	if (m_SliceMode & SliceMode_Horizontal)
	{
		width -= m_SliceRight - m_SliceLeft - 1;
		width += m_FillWidth;
	}
	return width;
}

int Image::GetHeight2(void)
{
	int height = m_Height;

	if (m_SliceMode & SliceMode_Vertical)
	{
		height -= m_SliceBottom - m_SliceTop - 1;
		height += m_FillHeight;
	}
	return height;
}


void Image::ResetSlicePoistion(void)
{
	m_SliceLeft = 0;
	m_SliceRight = 0;
	m_SliceTop = 0;
	m_SliceBottom = 0;

	m_FillWidth = 1;
	m_FillHeight = 1;
}

void Image::GetHLineData(unsigned int *pixels, int width, int height, unsigned int *line, int lineIndex)
{
	memcpy(line, pixels + lineIndex * width, width * sizeof(unsigned int));
}

void Image::GetVLineData(unsigned int *pixels, int width, int height, unsigned int *line, int lineIndex)
{
	for (int i = 0; i < height; i++)
	{
		line[i] = pixels[i * width + lineIndex];
	}
}

void Image::CalcSliceHor(unsigned int *pixels, int width, int height)
{
	unsigned int *prevLine = new unsigned int[height];
	unsigned int *curLine = new unsigned int[height];
	int sameLineStart = -1, sameLineEnd = -1;
	int sameLineStartFinal = -1;
	int sameLineNum = 0;

	GetVLineData(pixels, width, height, prevLine, 0);
	for (int i = 1; i < width; i++)
	{
		GetVLineData(pixels, width, height, curLine, i);

		if (memcmp(prevLine, curLine, height * sizeof(unsigned int)) == 0)
		{
			if (sameLineStart < 0)
				sameLineStart = i - 1;
		}
		else
		{
			if (sameLineStart >= 0)
			{
				sameLineEnd = i - 1;
				if (sameLineEnd - sameLineStart + 1 > sameLineNum)
				{
					sameLineStartFinal = sameLineStart;
					sameLineNum = sameLineEnd - sameLineStart + 1;

					sameLineStart = -1;
					sameLineEnd = -1;
				}
			}
			else
			{
				memcpy(prevLine, curLine, height * sizeof(unsigned int));
			}
		}
	}
	if (sameLineStart >= 0 && sameLineEnd < 0)
		sameLineEnd = width - 1;

	if (sameLineEnd - sameLineStart + 1 > sameLineNum)
	{
		sameLineStartFinal = sameLineStart;
		sameLineNum = sameLineEnd - sameLineStart + 1;
	}

	if (sameLineStartFinal >= 0)
	{
		this->m_SliceLeft = sameLineStartFinal;
		this->m_SliceRight = sameLineStartFinal + sameLineNum - 1;
		this->m_FillWidth = 1;
	}
	else
	{
		this->m_SliceLeft = width / 2;
		this->m_SliceRight = width / 2 + 1;
		this->m_FillWidth = 1;
	}

	delete [] prevLine;
	delete [] curLine;
}

void Image::CalcSliceVer(unsigned int *pixels, int width, int height)
{
	unsigned int *prevLine = new unsigned int[width];
	unsigned int *curLine = new unsigned int[width];
	int sameLineStart = -1, sameLineEnd = -1;
	int sameLineStartFinal = -1;
	int sameLineNum = 0;

	GetHLineData(pixels, width, height, prevLine, 0);
	for (int i = 1; i < height; i++)
	{
		GetHLineData(pixels, width, height, curLine, i);

		if (memcmp(prevLine, curLine, width * sizeof(unsigned int)) == 0)
		{
			if (sameLineStart < 0)
				sameLineStart = i - 1;
		}
		else
		{
			if (sameLineStart >= 0)
			{
				sameLineEnd = i - 1;

				if (sameLineEnd - sameLineStart + 1 > sameLineNum)
				{
					sameLineStartFinal = sameLineStart;
					sameLineNum = sameLineEnd - sameLineStart + 1;

					sameLineStart = -1;
					sameLineEnd = -1;
				}
			}
			else
			{
				memcpy(prevLine, curLine, width * sizeof(unsigned int));
			}
		}
	}
	if (sameLineStart >= 0 && sameLineEnd < 0)
		sameLineEnd = height - 1;

	if (sameLineEnd - sameLineStart + 1 > sameLineNum)
	{
		sameLineStartFinal = sameLineStart;
		sameLineNum = sameLineEnd - sameLineStart + 1;
	}

	if (sameLineStartFinal >= 0)
	{
		this->m_SliceTop = sameLineStartFinal;
		this->m_SliceBottom = sameLineStartFinal + sameLineNum - 1;
		this->m_FillWidth = 1;
	}
	else
	{
		this->m_SliceTop = height / 2;
		this->m_SliceBottom = height / 2 + 1;
		this->m_FillWidth = 1;
	}

	delete [] prevLine;
	delete [] curLine;
}

void Image::CalcSlicePosition(void)
{
	ResetSlicePoistion();

	if (this->m_SliceMode == SliceMode_None)
		return;

	wxImage image(m_FileName);
	if (!image.IsOk())
		return;

	unsigned char *pixels = image.GetData();
	unsigned char *alpha = image.HasAlpha() ? image.GetAlpha() : NULL;
	unsigned char a, r, g, b;

	unsigned int *pixel_buf = new unsigned int[image.GetWidth() * image.GetHeight()];
	if (pixel_buf == nullptr)
		return;

	for (int i = 0; i < image.GetWidth() * image.GetHeight(); i++)
	{
		a = alpha ? alpha[i] : 0xFF;
		r = pixels[i * 3];
		g = pixels[i * 3 + 1];
		b = pixels[i * 3 + 2];
		pixel_buf[i] = (a << 24) | (r << 16) | (g << 8) | b;
	}

	if (this->m_SliceMode & SliceMode_Horizontal)
	{
		CalcSliceHor(pixel_buf, image.GetWidth(), image.GetHeight());
	}
	if (this->m_SliceMode & SliceMode_Vertical)
	{
		CalcSliceVer(pixel_buf, image.GetWidth(), image.GetHeight());
	}

	delete [] pixel_buf;
}

unsigned int Image::GetImageDataSize(void)
{
	unsigned int size = GetWidth2() * GetHeight2() * GetPixelSize();

	return (size + 3) / 4 * 4;	/* must align with 4 */
}

int Image::GetColorMode2(void)
{
	if (m_ColorMode == ColorMode_Auto)
		return Image::ProjectColorMode;

	return m_ColorMode;
}

unsigned int Image::GetImagePixel(wxImage &image, int x, int y)
{
	int originWidth, originHeight;
	int width, height;
	int index;

	originWidth = image.GetWidth();
	originHeight = image.GetHeight();
	width = GetWidth2();
	height = GetHeight2();

	unsigned char *pixels = image.GetData();
	unsigned char *alpha = image.HasAlpha() ? image.GetAlpha() : NULL;
	unsigned char a, r, g, b;

	if ((m_SliceMode & SliceMode_Horizontal) && x > m_SliceLeft + m_FillWidth)
		x = m_SliceRight + x - m_SliceLeft - m_FillWidth - 1;

	if ((m_SliceMode & SliceMode_Vertical) && y > m_SliceTop + m_FillHeight)
		y = m_SliceBottom + y - m_SliceTop - m_FillHeight - 1;

	index = y * originWidth + x;

	a = alpha ? alpha[index] : 0xFF;
	r = pixels[index * 3];
	g = pixels[index * 3 + 1];
	b = pixels[index * 3 + 2];

	return (a << 24) | (r << 16) | (g << 8) | b;
}

void Image::GetImagePixels(unsigned char *buffer, unsigned int size, int *hasTrans)
{
	int colorMode = GetColorMode2();
	wxImage image(m_FileName);
	int i, j, index;
	unsigned int color;

	*hasTrans = 0;
	memset(buffer, 0xFF, size);

	for (j = 0; j < GetHeight2(); j++)
	{
		for (i = 0; i < GetWidth2(); i++)
		{
			index = j * GetWidth2() + i;
			color = GetImagePixel(image, i, j);

			if (colorMode == ColorMode_ARGB8888)
			{
				unsigned int *buf32 = (unsigned int *)buffer;

				buf32[index] = color;
				if (color & 0xFF000000 != 0xFF000000)
					*hasTrans = 1;
			}
			else if (colorMode == ColorMode_ARGB1555)
			{
				unsigned short *buf16 = (unsigned short *)buffer;
				unsigned short argb1555 = ToARGB1555(color);

				if (m_HaveKeyColor && ((argb1555 & 0x7FFF) == (ToARGB1555(m_KeyColor) & 0x7FFF)))
					argb1555 &= 0x7FFF;		/* mark alpha bit as 0 */

				if (!(argb1555 & 0x8000))
					*hasTrans = 1;

				buf16[i] = argb1555;
			}
		}
	}
}

void Image::GetPreviewPixels(unsigned int *pixels, wxSize size, wxColour bkgColor, int scalePercent)
{
	wxImage image(m_FileName);

	for (int y = 0; y < size.GetHeight(); y++)
	{
		for (int x = 0; x < size.GetWidth(); x++)
		{
			pixels[y * size.GetWidth() + x] = bkgColor.GetRGBA();
		}
	}

	if (!image.IsOk())
		return;

	/* scale to fit size */
	int scaleWidth = image.GetWidth() * scalePercent / 100;
	int scaleHeight = image.GetHeight() * scalePercent / 100;

	scaleWidth = wxMax(1, scaleWidth);
	scaleHeight = wxMax(1, scaleHeight);
	if (scaleWidth > size.GetWidth())
	{
		scaleWidth = size.GetWidth();
		scaleHeight = scaleWidth * image.GetHeight() / image.GetWidth();
	}
	if (scaleHeight > size.GetHeight())
	{
		scaleHeight = size.GetHeight();
		scaleWidth = scaleHeight * image.GetWidth() / image.GetHeight();
	}

	/* slice position */
	int sliceLeft = m_SliceLeft * scaleWidth / image.GetWidth();
	int sliceRight = m_SliceRight * scaleWidth / image.GetWidth();
	int sliceTop = m_SliceTop * scaleHeight / image.GetHeight();
	int sliceBottom = m_SliceBottom * scaleHeight / image.GetHeight();
	int fillRight = (m_SliceLeft + m_FillWidth) * scaleWidth / image.GetWidth();
	int fillBottom = (m_SliceTop + m_FillHeight) * scaleHeight / image.GetHeight();

	/* rescale */
	if (scaleWidth != image.GetWidth() || scaleHeight != image.GetHeight())
		image.Rescale(scaleWidth, scaleHeight);

	/* align center */
	int imageX = (size.GetWidth() - scaleWidth) / 2;
	int imageY = (size.GetHeight() - scaleHeight) / 2;

	/* calc pixels */
	int colorMode = GetColorMode2();
	unsigned char *imagePixels = image.GetData();
	unsigned char *alpha = image.HasAlpha() ? image.GetAlpha() : NULL;
	unsigned char a, r, g, b;
	for (int j = 0; j < scaleHeight; j++)
	{
		for (int i = 0; i < scaleWidth; i++)
		{
			int index = j * scaleWidth + i;
			a = alpha ? alpha[index] : 0xFF;
			r = imagePixels[index * 3];
			g = imagePixels[index * 3 + 1];
			b = imagePixels[index * 3 + 2];

			if (colorMode == ColorMode_ARGB1555)
			{
				r &= 0xF8; g &= 0xF8; b &= 0xF8;
				if ((a < 0x80) || (m_HaveKeyColor && (unsigned int)(r << 16 | g << 8 | b) == (m_KeyColor & 0x00F8F8F8)))
					a = 0;
			}

			if (m_SliceMode & SliceMode_Horizontal)
			{
				if (i == fillRight) {a = 0xFF; r = g = 0x00, b = 0xFF;}
				else if (i == sliceLeft || i == sliceRight) {a = 0xFF; r = 0xFF, g = b = 0x00;}
			}
			if (m_SliceMode & SliceMode_Vertical)
			{
				if (j == fillBottom) {a = 0xFF; r = g = 0x00, b = 0xFF;}
				else if (j == sliceTop || j == sliceBottom) {a = 0xFF; r = 0xFF, g = b = 0x00;}
			}

			pixels[(imageY + j) * size.GetWidth() + (imageX + i)] = BlendColor(a, r, g, b, bkgColor.GetRGBA());
		}
	}
}


int Image::GetPixelSize(void)
{
	return (GetColorMode2() == ColorMode_ARGB8888) ? 4 : 2;
}


//-------------------------------------------------------------------------------------------------
// ImageList
//-------------------------------------------------------------------------------------------------
ImageList::ImageList(void)
{

}

//ImageList::ImageList(const ImageList& anotherList)
//{
//	for (unsigned int i = 0; i < anotherList.Count(); i++)
//	{
////		Image *image = new Image(anotherList.GetImageAt(i));
////		*image = *(anotherList.GetImageAt(i));
////		AddImage(image);
//		AddImage(new Image(anotherList.GetImageAt(i)));
////		wxLogMessage(image->GetFileName());
//	}
//}

//ImageList::ImageList(const wxString& binfile)
//{
//
//}

ImageList::~ImageList(void)
{
	for (unsigned int i = 0; i < m_Images.size(); i++)
	{
		delete m_Images.at(i);
	}
}


void ImageList::AddImage(Image *image)
{
	wxVector<Image *>::iterator it;
	Image *item;
	bool inserted = false;

	for (it = m_Images.begin(); it != m_Images.end(); it++)
	{
		item = *it;
		/* insert before current item */
		if (image->GetName() < item->GetName())
		{
			m_Images.insert(it, image);
			inserted = true;
			break;
		}
	}
	if (!inserted)	/* bigger than all exist, add at end */
	{
		m_Images.push_back(image);
	}
}


bool ImageList::DeleteImage(Image *image)
{
	wxVector<Image *>::iterator it;
	Image *item;
	bool remove = false;

	if (nullptr == image)
	{
		return remove;
	}
	for (it = m_Images.begin(); it != m_Images.end(); it++)
	{
		item = *it;
		/*remove*/
		if (image->GetName() == item->GetName())
		{
			m_Images.erase(it);
			remove = true;
			break;
		}
	}
	return remove;
}


wxArrayString ImageList::GetNames(void)
{
	wxArrayString names;

	for (unsigned int i = 0; i < m_Images.size(); i++)
	{
		names.Add(m_Images.at(i)->GetName());
	}
	return names;
}

wxArrayInt ImageList::GetValues(void)
{
	wxArrayInt values;

	for (unsigned int i = 0; i < m_Images.size(); i++)
	{
		values.Add(i);
	}
	return values;
}

long ImageList::FindImage(wxString name)
{
	for (unsigned int i = 0; i < m_Images.size(); i++)
	{
		if (m_Images.at(i)->GetName() == name)
		{
			return i;
		}
	}
	return 0xFFFFFF;
}

Image* ImageList::GetImage(wxString name)
{
	for (unsigned int i = 0; i < m_Images.size(); i++)
	{
		if (m_Images.at(i)->GetName() == name)
		{
			return m_Images.at(i);
		}
	}
	return nullptr;
}

wxString ImageList::GetImageName(long imageValue)
{
	if ((unsigned int)imageValue < m_Images.size())
	{
		return m_Images.at(imageValue)->GetName();
	}
	return wxEmptyString;
}


void ImageList::ApplyToGUISystem(void)
{
	unsigned int i, bmpNum = m_Images.size();
//	BMP_ITEM *bmpInfos;

#if 0	// debug
	for (i = 0; i < bmpNum; i++)
	{
		wxLogMessage(m_Images.at(i)->GetName());
	}
#endif

	set_bmp_manager_bmp_num(bmpNum);

	unsigned int *bmpOffsets = new unsigned int[bmpNum];
	unsigned int *bmpLength = new unsigned int[bmpNum];
	unsigned int bmpsDataLength = 0;

	for (i = 0; i < bmpNum; i++)
	{
		bmpLength[i] = m_Images.at(i)->GetImageDataSize();
		bmpsDataLength += bmpLength[i];
	}
	bmpOffsets[0] = 0;
	for (i = 1; i < bmpNum; i++)
	{
		bmpOffsets[i] = bmpOffsets[i - 1] + bmpLength[i - 1];
	}

	if (allBmpData)
		delete allBmpData;

	allBmpData = new unsigned char[bmpsDataLength];
	Image *image;
	int hasTrans;
	for (i = 0; i < bmpNum; i++)
	{
		image = m_Images.at(i);

		m_Images.at(i)->GetImagePixels(allBmpData + bmpOffsets[i], bmpLength[i], &hasTrans);
		bmp_manager_add_bmp(i, dsU2C(image->GetName()), image->GetWidth2(), image->GetHeight2(), bmpOffsets[i], image->GetColorMode2(), hasTrans);
	}

	bmp_manager_set_mem_pixel_data(allBmpData, bmpsDataLength);
//	bmp_manager_add_bmp_finish();
}
