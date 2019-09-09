/*
 * utf8reader.h
 *
 *  Created on: 2019-5-13
 *      Author: Brant
 */

#ifndef _UTF8READER_H_
#define _UTF8READER_H_


#include <wx/defs.h>
#include <wx/string.h>


class wxString;

class Utf8Reader
{
public:
	Utf8Reader(const wxString& filename);

	bool IsOK(void) { return m_IsOK; }
	wxString GetWxStr() const;


protected:
	bool DetectEncoding(const wxString& filename);


private:
	bool Read(const wxString& filename);
	bool ConvertToWxString(const wxByte* buffer, size_t size);

    wxString m_ConvStr;
    bool m_IsOK;
};


#endif /* _UTF8READER_H_ */
