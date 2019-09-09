/*
 * imagelistbox.h
 *
 *  Created on: 2019-4-28
 *      Author: Brant
 */

#ifndef _IMAGE_LIST_BOX_H_
#define _IMAGE_LIST_BOX_H_


#include <wx/window.h>
#include <wx/vlbox.h>
#include "images.h"
//#include "logos.h"


class ImageListBox : public wxVListBox
{
public:
	ImageListBox(wxWindow *parent, wxWindowID id);
	~ImageListBox(void) { }

	void SetImages(ImageList* imageList);
	ImageList* GetImages(void) { return m_ImageList; }

	bool AddImage(Image* image);
//	bool AddLogo(Image* image);
	bool AddImage(void);

	Image* GetImage(unsigned int index);
	Image* GetImage(const wxString& name);
	void SelectImage(unsigned int index);

	bool UpdateImage(unsigned int index, Image image);
	bool UpdateImageName(unsigned int index, const wxString& name);
//	bool UpdateColorValue(unsigned int index, unsigned int value);

	void DeleteImage(unsigned int index);
	void DeleteImage(Image *image);
//	void SetLogo(LogoList* logoList);
//	Logo* GetLogo(unsigned int index);

private:
	ImageList *m_ImageList = nullptr;
 //   LogoList  *m_LogoList = nullptr;
	wxCoord OnMeasureItem(size_t n) const;							/* virtual method from <wxVListBox> */
	void OnDrawItem(wxDC &dc, const wxRect &rect, size_t n) const;	/* virtual method from <wxVListBox> */

	wxDECLARE_EVENT_TABLE();
};


#endif /* _IMAGE_LIST_BOX_H_ */
