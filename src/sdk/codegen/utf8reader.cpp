/*
 * utf8reader.cpp
 *
 *  Created on: 2019-5-13
 *      Author: Brant
 */

#include <stdlib.h>
#include <wx/file.h>
#include "codegen/utf8reader.h"


Utf8Reader::Utf8Reader(const wxString& filename)
{
	m_IsOK = Read(filename);
}


wxString Utf8Reader::GetWxStr() const
{
	return m_ConvStr;
}


bool Utf8Reader::Read(const wxString& filename)
{
	wxFile file(filename);
	if (!file.IsOpened())
		return false;

	size_t size = file.Length();
	if (size == 0)
	{
		file.Close();
		return false;
	}

	wxByte* buffer = (wxByte*) malloc(sizeof(wxByte) * size);
	if (!buffer)
	{
		file.Close();
		return false;
	}
	size_t readBytes = file.Read((void*)buffer, size);
	bool result = false;
	if (readBytes > 0)
		result = ConvertToWxString(buffer, size);

	file.Close();
	free(buffer);
	return result;
}

inline wxString makeStringNoNull(const wxWCharBuffer &wideBuff)
{
    wxString result(wideBuff);
    if (!result.empty())
    {
        wxString::size_type ii = result.find_last_not_of('\0');
        if (ii != wxString::npos)
            result.resize(ii + 1);
    }
    return result;
}

bool Utf8Reader::ConvertToWxString(const wxByte* buffer, size_t size)
{
    if (!buffer || size == 0)
        return false; // Nothing we can do...

    size_t outlen = 0;
    wxWCharBuffer wideBuff;
    // if possible use the special conversion-routines, they are much faster than wxCSCov (at least on linux)
	wxMBConvUTF8 conv;
	wideBuff = conv.cMB2WC((const char*)buffer, size, &outlen);

    if (outlen<=0)
    	return false;

	m_ConvStr = makeStringNoNull(wideBuff);
	return true; // Done.
}
