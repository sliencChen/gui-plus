/*
 * tinywxuni.h
 *
 *  Created on: 2019-4-15
 *      Author: Brant
 */

#ifndef TINY_WX_UNI_H_
#define TINY_WX_UNI_H_


class wxString;
class TiXmlDocument;


namespace TinyXML
{
    bool           LoadDocument(const wxString& filename, TiXmlDocument *doc);
    TiXmlDocument* LoadDocument(const wxString& filename);

    bool SaveDocument(const wxString& filename, TiXmlDocument* doc);
}


#endif /* TINY_WX_UNI_H_ */
