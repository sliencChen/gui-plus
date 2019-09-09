/*
 * util.cpp
 *
 *  Created on: 2019-4-15
 *      Author: Brant
 */

#include <string.h>
#include "sdk_common.h"
#include "tinywxuni.h"


// Return @c str as a proper unicode-compatible string
wxString dsC2U(const char* str)
{
#if wxUSE_UNICODE
    return wxString(str, wxConvUTF8);
#else
    return wxString(str);
#endif
}

// Return multibyte (C string) representation of the string
#if wxUSE_UNICODE
const char* dsU2C(const wxString& str)
{
	return str.mb_str(wxConvUTF8);
}
#else
const wxWX2MBbuf dsU2C(const wxString& str)
{
	return str.mb_str();
}
#endif


unsigned int HexStringValue(const char *string)
{
	if (strncmp(string, "0x", 2) == 0 || strncmp(string, "0X", 2) == 0)
		string += 2;

	if (strlen(string) > 8)
		return 0;

	unsigned int value = 0;
	for (unsigned int i = 0; i < strlen(string); i++)
	{
		unsigned char ch = string[i];

		if (ch >= '0' && ch <= '9')
		{
			value = (value << 4) + ch - '0';
		}
		else if (ch >= 'A' && ch <= 'F')
		{
			value = (value << 4) + 10 + ch - 'A';
		}
		else if (ch >= 'a' && ch <= 'f')
		{
			value = (value << 4) + 10 + ch - 'a';
		}
		else	/* invalid char */
		{
			return 0;
		}
	}
	return value;
}


// Save a TinyXML document correctly, even if the path contains unicode characters.
bool dsSaveTinyXMLDocument(TiXmlDocument* doc, const wxString& filename)
{
    return TinyXML::SaveDocument(filename, doc);
}


static bool WriteWxStringToFile(wxFile& f, const wxString& data, wxFontEncoding encoding, bool bom)
{
    const char* mark = nullptr;
    size_t mark_length = 0;
    if (bom)
    {
		mark = "\xEF\xBB\xBF";
		mark_length = 3;

		if (f.Write(mark, mark_length) != mark_length)
			return false;
    }

    if (data.length() == 0)
        return true;

#if defined(UNICODE) || defined(_UNICODE)
    size_t inlen = data.Len(), outlen = 0;
    wxCharBuffer mbBuff;
	wxMBConvUTF8 conv;
	mbBuff = conv.cWC2MB(data.c_str(), inlen, &outlen);

     // if conversion to chosen encoding succeeded, we write the file to disk
    if (outlen > 0)
        return f.Write(mbBuff, outlen) == outlen;

    return true;
#else

    // For ANSI builds, dump the char* to file.
    return f.Write(data.c_str(), data.Length()) == data.Length();

#endif
}

namespace platform
{
#if defined ( __WIN32__ ) || defined ( _WIN64 )
    // Yes this is ugly. Feel free to come up with a better idea if you have one.
    // Using the obvious wxRenameFile (or the underlying wxRename) is no option under Windows, since
    // wxRename is simply a fuckshit wrapper around a CRT function which does not work the way
    // the wxRename author assumes (MSVCRT rename fails if the target exists, instead of overwriting).
    inline bool move(wxString const& old_name, wxString const& new_name)
    {
        // hopefully I got the unintellegible conversion stuff correct... at least it seems to work...
        return ::MoveFileEx(wxFNCONV(old_name), wxFNCONV(new_name), MOVEFILE_REPLACE_EXISTING);
    }
#else
    inline bool move(wxString const& old_name, wxString const& new_name)
    {
        return ::wxRenameFile(old_name, new_name, true);
    };
#endif
}


bool dsSaveToFile(const wxString& name, const wxString& data, wxFontEncoding encoding, bool bom)
{
    if (wxFileExists(name) == false)
    {
        wxFile f(name, wxFile::write_excl);
        return WriteWxStringToFile(f, data, encoding, bom);
    }
    else
    {
        if (!wxFile::Access(name, wxFile::write))
            return false;

        wxString temp(name);
        temp.append(wxT(".temp"));

        wxStructStat buff;
        wxLstat( name, &buff );

        wxFile f;
        f.Create(temp, true, buff.st_mode);

        if (WriteWxStringToFile(f, data, encoding, bom))
        {
            f.Close();
            if (platform::move(temp, name))
            {
                return true;
            }
            else
            {
                wxString failed(name);
                failed.append(wxT(".save-failed"));
                platform::move(temp, failed);
            }
        }
        return false;
    }
}


unsigned short ToARGB1555(unsigned char a, unsigned char r, unsigned char g, unsigned char b)
{
	return ((((a) & 0x80) << 8) | (((r)>>3)<<10) | (((g)>>3)<<5) | ((b)>>3));
}

unsigned short ToARGB1555(unsigned int argb8888)
{
	unsigned char a, r, g, b;

	a = (unsigned char)((argb8888 & 0xFF000000) >> 24);
    r = (unsigned char)((argb8888 & 0x00FF0000) >> 16);
    g = (unsigned char)((argb8888 & 0x0000FF00) >> 8);
    b = (unsigned char)((argb8888 & 0x000000FF));

	return ToARGB1555(a, r, g, b);
}

#define ABS_SUBTRACT(x, y) 	((x) >= (y) ? ((x)-(y)) : ((y)-(x)))
#define CURCOLOR(alpha, front, back) ((front)>(back)?(back)+(ABS_SUBTRACT(front,back)*(alpha)>>8):(back)-(ABS_SUBTRACT(front,back)*(alpha)>>8))
unsigned int BlendColor(unsigned char a, unsigned char r, unsigned char g, unsigned char b, unsigned int bkgColor)
{
	U8 A1, R1, G1, B1;
	U8 A, R, G, B;

	A1 = (bkgColor & 0xff000000) >> 24;
	R1 = (bkgColor & 0x00ff0000) >> 16;
	G1 = (bkgColor & 0x0000ff00) >> 8;
	B1 = (bkgColor & 0x000000ff);

	A = 0xff ^ (((0xff ^ A1) * (0xff ^ a)) / 0xff);

	R = CURCOLOR(a, r, R1);
	G = CURCOLOR(a, g, G1);
	B = CURCOLOR(a, b, B1);

	return A << 24 | R << 16 | G <<8 | B;
}


wxColour UIntToColor(unsigned int value)
{
	unsigned char a, r, g, b;

	a = (value & 0xff000000) >> 24;
	r = (value & 0x00ff0000) >> 16;
	g = (value & 0x0000ff00) >> 8;
	b = (value & 0x000000ff);

	return wxColour(r, g, b, a);
}

unsigned int ColorToUInt(wxColour color)
{
	return color.Alpha() <<24 | color.Red()<<16 | color.Green()<< 8 | color.Blue();
}
