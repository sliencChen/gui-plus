#include <stdio.h>
#include "sdk_common.h"
#include <wx/filefn.h>
#include "logos.h"

Logo::Logo(const wxString& filename, const wxString& name)
{
    wxImage image(filename);

	Logo_IsOK = image.IsOk();
    wxLogMessage(wxT("in OnAdd  5----"));
	if (Logo_IsOK)
	{
	    wxLogMessage(wxT("in OnAdd succes 7----"));
		m_Width = image.GetWidth();
		m_Height = image.GetHeight();
		wxLogMessage(wxT("logo m_Width  = %d"),m_Width);
		wxLogMessage(wxT("logo m_Height  = %d"),m_Height);
	}
	wxLogMessage(wxT("in OnAdd  6----"));
    logo_Name = name;
    logo_FileName = filename;
}

unsigned int Logo::GetImageDataSize(void)
{
	return m_Width * m_Height * 4;
}

void Logo::GetImagePixels(unsigned char *buffer, unsigned int size)
{
	wxImage image(logo_FileName);

	unsigned char *pixels = image.GetData();
	unsigned char r, g, b;
	unsigned int *buf32 = (unsigned int *)buffer;

	for (unsigned int i = 0; i < (unsigned int)(image.GetWidth() * image.GetHeight()); i++)
	{
		r = pixels[i * 3];
		g = pixels[i * 3 + 1];
		b = pixels[i * 3 + 2];
		buf32[i] = (0xFF << 24) | (r << 16) | (g << 8) | b;
	}
}

void Logo::GetPreviewPixels(unsigned int *pixels, wxSize size, wxColour bkgColor)
{
	wxImage image(logo_FileName);

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
	int scaleWidth = image.GetWidth();
	int scaleHeight = image.GetHeight();
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
	if (scaleWidth != image.GetWidth() || scaleHeight != image.GetHeight())
		image.Rescale(scaleWidth, scaleHeight);

	/* align center */
	int imageX = (size.GetWidth() - scaleWidth) / 2;
	int imageY = (size.GetHeight() - scaleHeight) / 2;

	int colorMode = ColorMode_ARGB8888;

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
			pixels[(imageY + j) * size.GetWidth() + (imageX + i)] = BlendColor(a, r, g, b, bkgColor.GetRGBA());
		}
	}
}

LogoList::LogoList(void)
{

}

LogoList::~LogoList(void)
{
	for (unsigned int i = 0; i < logo_Images.size(); i++)
	{
		delete logo_Images.at(i);
	}
}

void LogoList::AddLogo(Logo *logo)
{
   	wxVector<Logo *>::iterator it;
	Logo *item;
	bool inserted = false;

	for (it = logo_Images.begin(); it != logo_Images.end(); it++)
	{
		item = *it;
		/* insert before current item */
		if (logo->GetName() < item->GetName())
		{
		    //wxLogMessage(wxT("in AddLogo 2----"));
			logo_Images.insert(it, logo);
			inserted = true;
			break;
		}
	}
	if (!inserted)	/* bigger than all exist, add at end */
	{
	   // wxLogMessage(wxT("in AddLogo 3----"));
		logo_Images.push_back(logo);
	}
}

void LogoList::DeleteLogo(Logo *logo)
{

    wxVector<Logo *>::iterator it;
	Logo *item;

	for (it = logo_Images.begin(); it != logo_Images.end(); it++)
	{
		item = *it;
		wxString message = "DeleteLogo ";
        message += item->GetName();
        message += logo->GetName();
        wxLogMessage(message);
		/* insert before current item */
		if (logo->GetName().IsSameAs(item->GetName()))
		{
		    wxLogMessage(wxT("in DeleteLogo 2----"));
			logo_Images.erase(it);
			break;
		}
	}
}

void LogoList::UpLogo(Logo *logo)
{
    wxVector<Logo *>::iterator it;
	Logo *item;

	for (it = logo_Images.begin(); it != logo_Images.end(); it++)
	{
	    wxLogMessage(wxT("in DeleteLogo 1----"));
		item = *it;
		/* insert before current item */
		if (logo->GetName().IsSameAs(item->GetName()))
		{
		    wxLogMessage(wxT("in DeleteLogo 2----"));
		    logo_Images.erase(it);
		    if (--it >= logo_Images.begin())
            {
                logo_Images.insert(it,logo);
            }
            else
            {
                logo_Images.insert(logo_Images.end(),logo);
            }
			break;
		}
	}
}

void LogoList::DownLogo(Logo *logo)
{
    wxVector<Logo *>::iterator it;
	Logo *item;

	for (it = logo_Images.begin(); it != logo_Images.end(); it++)
	{
	    wxLogMessage(wxT("in DeleteLogo 1----"));
		item = *it;
		/* insert before current item */
		if (logo->GetName().IsSameAs(item->GetName()))
		{
		    wxLogMessage(wxT("in DeleteLogo 2----"));
		    logo_Images.erase(it);
		    if (++it > logo_Images.end())
            {
                logo_Images.insert(logo_Images.begin(),logo);
            }
            else
            {
                logo_Images.insert(it,logo);
            }
			break;
		}
	}
}

wxArrayString LogoList::GetNames(void)
{
	wxArrayString names;

	for (unsigned int i = 0; i < logo_Images.size(); i++)
	{
		names.Add(logo_Images.at(i)->GetName());
	}
	return names;
}

wxArrayInt LogoList::GetValues(void)
{
	wxArrayInt values;

	for (unsigned int i = 0; i < logo_Images.size(); i++)
	{
		values.Add(i);
	}
	return values;
}

long LogoList::FindLogo(wxString name)
{
	for (unsigned int i = 0; i < logo_Images.size(); i++)
	{
		if (logo_Images.at(i)->GetName() == name)
		{
			return i;
		}
	}
	return 0xFFFFFF;
}

wxString LogoList::GetImageName(long imageValue)
{
	if ((unsigned int)imageValue < logo_Images.size())
	{
		return logo_Images.at(imageValue)->GetName();
	}
	return wxEmptyString;
}

wxString LogoList::GetImageFileName(long imageValue)
{
	if ((unsigned int)imageValue < logo_Images.size())
	{
		return logo_Images.at(imageValue)->GetFileName();
	}
	return wxEmptyString;
}

void LogoList::ApplyToGUISystem(void)
{
	unsigned int i, bmpNum = logo_Images.size();

#if 0	// debug
	for (i = 0; i < bmpNum; i++)
	{
		wxLogMessage(m_Images.at(i)->GetName());
	}
#endif

	for (i = 0; i < bmpNum; i++)
	{
		unsigned int dataSize = logo_Images.at(i)->GetImageDataSize();
		unsigned char *logoData = new unsigned char[dataSize];

		if(logoData == nullptr)
		{
			continue;
		}
		logo_Images.at(i)->GetImagePixels(logoData, dataSize);
		set_logo_data(i, logoData, dataSize);
		delete logoData;
	}
}

