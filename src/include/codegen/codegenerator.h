/*
 * codegenerator.h
 *
 *  Created on: 2019-5-6
 *      Author: Brant
 */

#ifndef _CODEGEN_CODEGENERATOR_H_
#define _CODEGEN_CODEGENERATOR_H_


#include "codegen/codercontext.h"
//#include <prep.h>

/** \brief Base class for items which generate source code */
class wxsCodeGenerator
{
    public:

        /** \brief Ctor */
        wxsCodeGenerator();

        /** \brief Dctor */
        virtual ~wxsCodeGenerator();

        /** \brief Request to build the code */
        virtual void BuildCode(wxsCoderContext* Context);

    protected:

        virtual void OnBuildDeclarationsCode() = 0;          ///< \brief Building variable declaration / forward declaration
        virtual void OnBuildWindowStructureCode() = 0;		 ///< \brief Building window structure
        virtual void OnBuildCreatingCode() = 0;              ///< \brief Building code manually creating resource, it may also be used to add header files
//        virtual void OnBuildHeadersCode() {}                 ///< \brief Building headers arrays, it's not required since headers can be added in OnBuildCreatingCode()
//        virtual void OnBuildEventsConnectingCode() = 0;      ///< \brief Building code connecting events
        virtual void OnBuildIdCode() = 0;                    ///< \brief Building code declaring class members and values of identifiers
//        virtual void OnBuildXRCFetchingCode() = 0;           ///< \brief Building code which fetches this item from xrc resource
//        virtual void OnUpdateFlags(cb_unused long& Flags) {} ///< \brief Function called to update context flags just before the code is generated, previous flags are restored after code is generated

        /** \brief Getting current coding context */
        inline wxsCoderContext* GetCoderContext() { return m_Context; }

        /** \brief Getting current language */
        inline CodingLanguage GetLanguage() { return m_Context ? m_Context->m_Language : CodingLanguage_Unknown; }

        /** \brief Getting flags of current coding context */
        inline long GetCoderFlags() { return m_Context ? m_Context->m_Flags : 0; }

        /** \brief Adding header file into current context */
        inline void AddHeader(const wxString& Header,const wxString& DeclaredClass,long HeaderFlags=0)
        {
            if ( GetCoderContext() ) GetCoderContext()->AddHeader(Header,DeclaredClass,HeaderFlags);
        }

        /*** \brief Adding forward declaration of item's class */
        inline void AddDeclaration(const wxString& Declaration)
        {
            if ( GetCoderContext() ) GetCoderContext()->AddDeclaration(Declaration);
        }

        /*** \brief Adding window structure of item's class */
        inline void AddStructure(const wxString& Structure)
        {
        	if ( GetCoderContext() ) GetCoderContext()->AddStructure(Structure);
        }

        /** \brief Adding XRC fetching code */
        inline void AddXRCFetchingCode(const wxString& Code)
        {
            if ( GetCoderContext() ) GetCoderContext()->AddXRCFetchingCode(Code);
        }

        /** \brief Adding Manually building code */
        inline void AddBuildingCode(const wxString& Code)
        {
            if ( GetCoderContext() ) GetCoderContext()->AddBuildingCode(Code);
        }

        /** \brief Adding event connecting code */
        inline void AddEventCode(const wxString& Code)
        {
            if ( GetCoderContext() ) GetCoderContext()->AddEventCode(Code);
        }

        /** \brief Adding id-generating code */
        inline void AddIdCode(const wxString& Enumeration,const wxString& Initialization)
        {
            if ( GetCoderContext() ) GetCoderContext()->AddIdCode(Enumeration,Initialization);
        }

    private:

        wxsCoderContext* m_Context;
};


#endif /* _CODEGEN_CODEGENERATOR_H_ */
