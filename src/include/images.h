/*
 * images.h
 *
 *  Created on: 2019-4-24
 *      Author: Brant
 */

#ifndef _IMAGES_H_
#define _IMAGES_H_


enum
{
	SliceMode_None = 0,
	SliceMode_Horizontal = 1,
	SliceMode_Vertical = 2,
	SliceMode_HV_Both = 3,
	SliceMode_Max
};

enum
{
	SliceFillMode_Tile = 0,
	SliceFillMode_Stretch = 1,
	SliceFillMode_Max
};


class Image
{
public:
	static int ProjectColorMode;


public:
	Image(const Image* image);
	Image(const wxString& filename, const wxString& name);

	wxString GetFileName(void) const { return m_FileName; }

	void SetName(const wxString& name) { m_Name = name; }
	wxString GetName(void) const { return m_Name; }

	int GetWidth(void) const { return m_Width; }
	int GetWidth2(void);
	int GetHeight(void) const { return m_Height; }
	int GetHeight2(void);

	bool IsOK(void) const { return m_IsOK; }

	/* color */
	void SetColorMode(int mode) { m_ColorMode = mode; }
	int GetColorMode(void) const { return m_ColorMode; }
	int GetColorMode2(void);

	void SetKeyColor(unsigned int color) { m_HaveKeyColor = true; m_KeyColor = color; }
	unsigned int GetKeyColor(void) const { return m_KeyColor; }
	void SetUseKeyColor(bool use) { m_HaveKeyColor = use; }
	bool HaveKeyColor(void) const { return m_ColorMode != ColorMode_ARGB8888 && m_HaveKeyColor; }

	/* slice */
	int GetSliceMode(void) const { return m_SliceMode; }
	void SetSliceMode(int mode) { m_SliceMode = mode; }

	int GetSliceLeft(void) const { return m_SliceLeft; }
	void SetSliceLeft(int left) { m_SliceLeft = left; }

	int GetSliceRight(void) const { return m_SliceRight; }
	void SetSliceRight(int right) { m_SliceRight = right; }

	int GetSliceTop(void) const { return m_SliceTop; }
	void SetSliceTop(int top) { m_SliceTop = top; }

	int GetSliceBottom(void) const { return m_SliceBottom; }
	void SetSliceBottom(int bottom) { m_SliceBottom = bottom; }

	int GetSliceFillMode(void) const { return m_FillMode; }
	void SetSliceFillMode(int mode) { m_FillMode = mode; }

	int GetSliceFillWidth(void) const { return m_FillWidth; }
	void SetSliceFillWidth(int width) { m_FillWidth = width; }

	int GetSliceFillHeight(void) const { return m_FillHeight; }
	void SetSliceFillHeight(int height) { m_FillHeight = height; }

	void CalcSlicePosition(void);


	/* for stb gui system, return pixel buffer, need delete it after use */
	unsigned int GetImageDataSize(void);
	void GetImagePixels(unsigned char *buffer, unsigned int size, int *hasTrans);
	void GetPreviewPixels(unsigned int *pixels, wxSize size, wxColour bkgColor, int scalePercent=100);


private:
	int GetPixelSize(void);
	void ResetSlicePoistion(void);
	void GetHLineData(unsigned int *pixels, int width, int height, unsigned int *line, int lineIndex);
	void GetVLineData(unsigned int *pixels, int width, int height, unsigned int *line, int lineIndex);
	void CalcSliceHor(unsigned int *pixels, int width, int height);
	void CalcSliceVer(unsigned int *pixels, int width, int height);

	unsigned int GetImagePixel(wxImage &image, int x, int y);


	wxString m_FileName;
	wxString m_Name;
	int m_Width = 0;
	int m_Height = 0;
	bool m_IsOK = false;

	/*color mode */
	int m_ColorMode;
	bool m_HaveKeyColor = false;
	unsigned int m_KeyColor;

	/* slice */
	int m_SliceMode = SliceMode_None;

	int m_SliceLeft = 0;
	int m_SliceRight = 0;
	int m_SliceTop = 0;
	int m_SliceBottom = 0;

	int m_FillMode = SliceFillMode_Tile;
	int m_FillWidth = 1;
	int m_FillHeight = 1;
};



//-------------------------------------------------------------------------------------------------
// ImageList
//-------------------------------------------------------------------------------------------------
class ImageList
{
public:
	ImageList(void);
//	ImageList(const ImageList& anotherList);
//	ImageList(const wxString& binfile);
	~ImageList(void);

	void AddImage(Image *image);
	bool DeleteImage(Image *image);
	unsigned int Count(void) const { return m_Images.size(); }
	Image* GetImageAt(unsigned int i) const
	{
		if (i >= m_Images.size())
			return nullptr;
		return m_Images.at(i);
	}

	wxArrayString GetNames(void);
	wxArrayInt GetValues(void);

	long FindImage(wxString name);
	Image* GetImage(wxString name);
	wxString GetImageName(long imageValue);

	void ApplyToGUISystem(void);


private:
	wxVector<Image *> m_Images;
};


#endif /* _IMAGES_H_ */
