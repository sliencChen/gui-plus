/*
 * util.h
 *
 *  Created on: 2019-4-15
 *      Author: Brant
 */

#ifndef DESIGNER_UTIL_H_
#define DESIGNER_UTIL_H_


wxString dsC2U(const char* str);
#if wxUSE_UNICODE
const char* dsU2C(const wxString& str);
#else
const wxWX2MBbuf dsU2C(const wxString& str);
#endif


unsigned int HexStringValue(const char *string);


// Save a TinyXML document correctly, even if the path contains unicode characters.
bool dsSaveTinyXMLDocument(TiXmlDocument* doc, const wxString& filename);
bool dsSaveToFile(const wxString& name, const wxString& data, wxFontEncoding encoding, bool bom);


unsigned short ToARGB1555(unsigned char a, unsigned char r, unsigned char g, unsigned char b);
unsigned short ToARGB1555(unsigned int argb8888);
unsigned int BlendColor(unsigned char a, unsigned char r, unsigned char g, unsigned char b, unsigned int bkgColor);

wxColour UIntToColor(unsigned int value);
unsigned int ColorToUInt(wxColour color);


#endif /* DESIGNER_UTIL_H_ */
