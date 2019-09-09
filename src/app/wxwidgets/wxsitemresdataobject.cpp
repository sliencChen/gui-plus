/*
 * wxsitemresdataobject.cpp
 *
 *  Created on: 2019-7-14
 *      Author: ZhangWei
 */

#include "wxsitemresdataobject.h"
#include "manager.h"
#include <wx/buffer.h>
#include <sstream>

#define cb_unused              __attribute__ ((__unused__))
#define wxWX2MBbuf wxCharBuffer


wxsItemResDataObject::wxsItemResDataObject(): m_ItemCount(0)
{
    Clear();
}

wxsItemResDataObject::~wxsItemResDataObject()
{
}

void wxsItemResDataObject::GetAllFormats(wxDataFormat *formats, cb_unused Direction dir) const
{
    formats[0] = wxDataFormat(wxDF_TEXT);
    formats[1] = wxDataFormat(wxsDF_WIDGET);
}

bool wxsItemResDataObject::GetDataHere(cb_unused const wxDataFormat& format,void *buf) const
{
    wxString XmlData = GetXmlData();
    const wxString str = dsC2U(XmlData);
    memcpy(buf,str,strlen(str)+1);
       return true;
}

size_t wxsItemResDataObject::GetDataSize(cb_unused const wxDataFormat& format) const
{
    wxString XmlData = GetXmlData();
    const wxString str = dsC2U(XmlData);
    return strlen(str)+1;
}

size_t wxsItemResDataObject::GetFormatCount(cb_unused Direction dir) const
{
    return 2;
}

wxDataFormat wxsItemResDataObject::GetPreferredFormat(cb_unused Direction dir) const
{
    return wxDataFormat(wxsDF_WIDGET);
}

bool wxsItemResDataObject::SetData(cb_unused const wxDataFormat& format, size_t len, const void *buf)
{
    char* CharBuff = new char[len+1];
    memcpy(CharBuff,buf,len);

    // Just to make sure we won't go out of buffer
    CharBuff[len] = '\0';

    bool Ret = SetXmlData(dsC2U(CharBuff));
    delete[] CharBuff;
    return Ret;
}

void wxsItemResDataObject::Clear()
{
    m_XmlDoc.Clear();
    m_XmlElem = m_XmlDoc.InsertEndChild(TiXmlElement("resource"))->ToElement();
    m_ItemCount = 0;
}

void wxsItemResDataObject::AddPageObjectElement(uiWindow *window, TiXmlElement* parent)
{
	if (window == nullptr)
		return;

	TiXmlElement* element = window->GetXMLElement();
	parent->LinkEndChild(element);
    m_ItemCount++;

	uiWindow *child = window->m_Childs;
	while (child)
	{
		AddPageObjectElement(child, element);
		child = child->m_Next;
	}
}

bool wxsItemResDataObject::AddItem(uiWindow* Item)
{
    if ( !Item ) return false;

    AddPageObjectElement(Item, m_XmlElem);
    return true;
}

int wxsItemResDataObject::GetItemCount() const
{
    return m_ItemCount;
}

bool wxsItemResDataObject::LoadFromClipBoard(uiWindow* m_ActiveWindow, wxTreeCtrl *WindowTreeControl,PageFile *ActivePage)
{
	/* load all ui objects */
    TiXmlElement* rootObject;
    rootObject = m_XmlElem->FirstChildElement("object");

    LoadPageObject(rootObject, m_ActiveWindow, WindowTreeControl, ActivePage);

	return true;
}

int wxsItemResDataObject::CheckIsItemWindow(uiWindow* m_ActiveWindow, uiWindow* m_CopyWindow)
{
	if (m_CopyWindow->GetClassName().IsSameAs(wxT("UIEditItem")))
	{
		if (m_ActiveWindow->GetClassName().IsSameAs(wxT("UIEdit")))
			return true;
		else
		{
			wxMessageBox(wxT("You must Paste in Edit control"));
			return -2;
		}
	}
	else if (m_CopyWindow->GetClassName().IsSameAs(wxT("UIMenuItem")))
	{
		if (m_ActiveWindow->GetClassName().IsSameAs(wxT("UIMenu")))
			return true;
		else
		{
			wxMessageBox(wxT("You must Paste in Menu control"));
			return -2;
		}

	}
	return false;
}

int wxsItemResDataObject::ClassId(const char *className)
{
	const char* supportedClass[] =
	{
			"UIPage", 		"UILabel",		"UIImage",	"UIList",
			"UIEdit",		"UIEditItem",	"UIMenu",	"UIMenuItem",
			"UIHelp",		"UILineChart",	"UIPrompt",	"UIMessageBox",
			"UITextField", 	"UIListSizer",	"UIBoxSizer"
	};

	for (unsigned int i = 0; i < sizeof(supportedClass) / sizeof(supportedClass[0]); i++)
	{
		if (strcmp(className, supportedClass[i]) == 0)
			return i;
	}
	return -1;
}

void wxsItemResDataObject::LoadPageObject(TiXmlElement* element, uiWindow *parent, wxTreeCtrl *WindowTreeControl,PageFile *ActivePage)
{
    int classId;
    int firsttime = 1;
    wxString Name;
    wxTreeItemIdValue cookie;
	wxTreeItemId root = WindowTreeControl->GetRootItem();
	wxTreeItemId item;
	if ((element == nullptr) || (parent == nullptr))
        return;

	while (element)
	{
		const char* nodeClass = element->Attribute("class");
		uiWindow *window = WindowFactory::Create(nodeClass);
		window->SetProperties(element);
        classId = ClassId(nodeClass);
again:
        if(1 == firsttime)
        {
            Name = window->GetStrPropValue(PROP_NAME);
            firsttime++;
        }
        else
        {
            switch (classId)
            {
                case 0:
                    Name = ActivePage->GetCoderContext()->GetUniqueName(_T("Page"));
                    break;
                case 1:
                    Name = ActivePage->GetCoderContext()->GetUniqueName(_T("Label"));
                    break;
                case 2:
                    Name = ActivePage->GetCoderContext()->GetUniqueName(_T("Image"));
					break;
				case 3:
					Name = ActivePage->GetCoderContext()->GetUniqueName(_T("List"));
					break;
				case 4:
                    Name = ActivePage->GetCoderContext()->GetUniqueName(_T("Edit"));
					break;
				case 5:
                    Name = ActivePage->GetCoderContext()->GetUniqueName(_T("Edit_Item"));
					break;
				case 6:
                    Name = ActivePage->GetCoderContext()->GetUniqueName(_T("Menu"));
					break;
				case 7:
                    Name = ActivePage->GetCoderContext()->GetUniqueName(_T("Menu_Item"));
                    break;
				case 9:
					Name = ActivePage->GetCoderContext()->GetUniqueName(_T("LineChart"));
					break;
				case 10:
					Name = ActivePage->GetCoderContext()->GetUniqueName(_T("Prompt"));
					break;
				case 11:
					Name = ActivePage->GetCoderContext()->GetUniqueName(_T("Message_Box"));
					break;
				case 12:
					Name = ActivePage->GetCoderContext()->GetUniqueName(_T("Text_Field"));
					break;
				case 13:
					Name = ActivePage->GetCoderContext()->GetUniqueName(_T("ListSizer"));
					break;
				case 14:
					Name = ActivePage->GetCoderContext()->GetUniqueName(_T("BoxSizer"));
					break;
                default:
                    break;
            }
        }
        for (item = WindowTreeControl->GetFirstChild(root, cookie); item.IsOk(); item = WindowTreeControl->GetNextVisible(item))
        {
            if (WindowTreeControl->GetItemText(item) == Name)
            {
                goto again;
            }
        }
        window->SetStringProperty(PROP_NAME,	(char *)dsU2C(Name));

        parent->AddSubWindow(window);

		/* add all child */
		LoadPageObject(element->FirstChildElement("object"), window, WindowTreeControl,ActivePage);

		element = element->NextSiblingElement("object");
	}
}

bool wxsItemResDataObject::SetXmlData(const wxString& Data)
{
    m_XmlDoc.Clear();
    m_ItemCount = 0;
    m_XmlDoc.Parse(dsC2U(Data));
    if ( m_XmlDoc.Error() )
    {
//        Manager::Get()->GetLogManager()->DebugLog(_T("wxSmith: Error loading Xml data -> ") + dsC2U(m_XmlDoc.ErrorDesc()));
        Clear();
        return false;
    }

    m_XmlElem = m_XmlDoc.FirstChildElement("resource");
    if ( !m_XmlElem )
    {
        Clear();
        return false;
    }

    for ( TiXmlElement* Elem = m_XmlElem->FirstChildElement("object");
          Elem;
          Elem = Elem->NextSiblingElement("object") )
    {
        m_ItemCount++;
    }

    return true;
}

wxString wxsItemResDataObject::GetXmlData() const
{
    TiXmlPrinter Printer;
    m_XmlDoc.Accept(&Printer);
    return dsC2U(Printer.CStr());
}
