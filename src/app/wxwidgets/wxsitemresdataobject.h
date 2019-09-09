/*
 * wxsitemresdataobject.h
 *
 *  Created on: 2019-7-14
 *      Author: ZhangWei
 */

#ifndef WXSITEMRESDATAOBJECT_H
#define WXSITEMRESDATAOBJECT_H

#include <wx/dataobj.h>
#include <tinyxml.h>
#include <wx/treectrl.h>
#include "window/uiWindow.h"
#include "pagefile.h"


#define wxsDF_WIDGET   _T("wxSmith XML")

//class wxsItem;
class wxsItemResData;

/** \brief Class representing one or more items with resource structure using wxDataObject class */
class wxsItemResDataObject : public wxDataObject
{
    public:

        /** \brief Ctor */
        wxsItemResDataObject();

        /** \brief Dctor */
        virtual ~wxsItemResDataObject();

        //=====================================
        // Operating on data
        //=====================================

        /** \brief Clearing all data */
        void Clear();

        void AddPageObjectElement(uiWindow *window, TiXmlElement* parent);

        /** \brief Adding widget into this data object */
        bool AddItem(uiWindow* Item);

        /** \brief Getting number of handled widgets inside this object */
        int GetItemCount() const;

        /** \brief Building wxsItem class from this data object
         *  \param Resource - resource owning item
         *  \param Index - id of item (in range 0..GetWidgetCount()-1)
         *  \return created item or 0 on error
         */
         bool LoadFromClipBoard(uiWindow* Item, wxTreeCtrl *WindowTreeControl,PageFile *ActivePage);
         void LoadPageObject(TiXmlElement* element, uiWindow *parent, wxTreeCtrl *WindowTreeControl,PageFile *ActivePage);

        /** \brief Setting Xml string describing widget */
        bool SetXmlData(const wxString& Data);

        /** \brief Getting Xml string describing widget */
        wxString GetXmlData() const;

        //=====================================
        // Members of wxDataObject class
        //=====================================

        /** \brief Enumerating all data formats.
         *
         * Formats available for reading and writing:
         * - wxDF_TEXT
         * - internal type ("wxSmith XML")
         */
        virtual void GetAllFormats(wxDataFormat *formats,Direction dir) const;

        /** \brief Copying data to raw buffer */
        virtual bool GetDataHere(const wxDataFormat& format,void *buf) const;

        /** \brief Returns number of data bytes */
        virtual size_t GetDataSize(const wxDataFormat& format) const;

        /** \brief Returns number of supported formats (in both cases - 2) */
        virtual size_t GetFormatCount(Direction dir) const;

        /** \brief Returning best format - "wxSmith XML" */
        virtual wxDataFormat GetPreferredFormat(Direction dir) const;

        /** \brief Setting data - will load Xml data */
        virtual bool SetData(const wxDataFormat& format,size_t len,const void *buf);

        int CheckIsItemWindow(uiWindow* m_ActiveWindow, uiWindow* m_CopyWindow);

    private:
        int ClassId(const char *className);

        TiXmlDocument m_XmlDoc;
        TiXmlElement* m_XmlElem;
        int m_ItemCount;
};

#endif
