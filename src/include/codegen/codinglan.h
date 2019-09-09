/*
 * codinglang.h
 *
 *  Created on: 2019-5-6
 *      Author: Brant
 */

#ifndef _CODEGEN_CODINGLANG_H_
#define _CODEGEN_CODINGLANG_H_

#include <wx/string.h>


typedef enum
{
	CodingLanguage_CPP 		= 0x0001,
	CodingLanguage_Unknown  = 0x8000
} CodingLanguage;


/** \brief Namespace with useful common functions for supported coding languages */
namespace CodeMarks
{
    /** \brief Getting wxString name of  coding language */
    wxString Name(CodingLanguage Id);

    /** \brief Getting id of coding language's name */
    CodingLanguage Id(const wxString& Name);

    /** \brief Getting coding language from file's extension */
    CodingLanguage IdFromExt(const wxString& Extension);

    /** \brief Helper function for generating block begin */
    wxString Beg(CodingLanguage Lang, const wxString& BlockName);

    /** \brief Helper function for generating block begin */
    wxString Beg(CodingLanguage Lang, const wxString& BlockName, const wxString& Param);

    /** \brief Helper function for generating block end */
    wxString End(CodingLanguage Lang);

    /** \brief Converting given string into representation of string in source code */
    wxString String(CodingLanguage Lang, const wxString& Source);

    /** \brief Converting given string into Wx-like representation */
    wxString WxString(CodingLanguage Lang, const wxString& Source, bool WithTranslation);

    /** \brief Checking if given string can be valid identifier in selected language
     *         (can be name of variable)
     */
    bool ValidateIdentifier(CodingLanguage Lang, const wxString& Identifier);

    /** \brief Posting notification message about unknown coding language */
    void Unknown(const wxString& Function, CodingLanguage Lang);
}


#endif /* _CODEGEN_CODINGLANG_H_ */
