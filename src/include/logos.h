#ifndef _LOGOS_H_
#define _LOGOS_H_


#include <gdi/resource.h>


class Logo
{
	public:
		Logo(const wxString& filename, const wxString& name);
	    wxString GetName(void) const { return logo_Name; }
	    wxString GetFileName(void) const { return logo_FileName; }
	    void GetPreviewPixels(unsigned int *pixels, wxSize size, wxColour bkgColor);
	    void GetImagePixels(unsigned char *buffer, unsigned int size);
		unsigned int GetImageDataSize(void);
	    bool IsOK(void) const { return Logo_IsOK; }
		
	private:
	    U8 GetPixelSize(void) { return 4;}
		wxString logo_Name;
		wxString logo_FileName;
	    int m_Width = 0;
		int m_Height = 0;
		bool Logo_IsOK = false;
};



//-------------------------------------------------------------------------------------------------
// LogoList
//-------------------------------------------------------------------------------------------------
class LogoList
{
	public:
		LogoList(void);

	    ~LogoList(void);
		void AddLogo(Logo *logo);
		unsigned int Count(void) const { return logo_Images.size(); }
		Logo* GetLogoAt(unsigned int i) const
		{
			if (i >= logo_Images.size())
				return nullptr;
			return logo_Images.at(i);
		}
	    wxArrayString GetNames(void);
		wxArrayInt GetValues(void);

		long FindLogo(wxString name);
		void DeleteLogo(Logo *logo);
		void UpLogo(Logo *logo);
		void DownLogo(Logo *logo);
		
		wxString GetImageName(long imageValue);
		wxString GetImageFileName(long imageValue);
		void ApplyToGUISystem(void);
	private:
		wxVector<Logo *> logo_Images;
};


#endif /* _LOGOS_H_ */

