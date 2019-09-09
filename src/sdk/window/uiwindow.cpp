/*
 * uiwindow.cpp
 *
 *  Created on: 2019-4-10
 *      Author: Brant
 */

#include <stdlib.h>
#include <string.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/advprops.h>
#include "sdk_common.h"
#include "colors.h"
#include <window/uiwindow.h>

#include <window/uipage.h>
#include <window/uilabel.h>
#include <window/uiimage.h>
#include <window/uilist.h>
#include <window/uiline_chart.h>
#include <window/uiedit.h>
#include <window/uiedit_item.h>
#include <window/uimenu.h>
#include <window/uimenu_item.h>
#include <window/uiprompt.h>
#include <window/uimessage_box.h>
#include <window/uitext_field.h>
#include <window/uibox_sizer.h>
#include <window/uilist_sizer.h>



uiWindow::uiWindow(uiWindow *parent)
{
	m_Properties.insert(pair<wxString, PropertyBase*>("Window Property", new PropertyFold("Window Property")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_CLASS, new PropertyString("UIWindow", false)));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_NAME, new PropertyString(RandomId())));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_VAR_NAME, new PropertyString("Window")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_BKG_COLOR, new PropertyColor()));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_POSITION, new PropertyRect())) ;

	if (parent)
		parent->AddSubWindow(this);
}

uiWindow::~uiWindow(void)
{
	Linked_map<wxString, PropertyBase*>::iterator it;
	for (it = m_Properties.begin(); it != m_Properties.end(); ++it)
	{
		wxString key = it->first;
		PropertyBase *prop = it->second;

		if (prop) delete prop;
	}
}


wxString uiWindow::Id(void)
{
	PropertyBase *prop = m_Properties.find(PROP_NAME)->second;

	if (prop)
	{
		return prop->toString();
	}
	return "undefined_id";
}

UIWindow* uiWindow::guiWindow(void)
{
	UIWindow *window = create_main_window("window", 100, 100, 100, 100);

	return window;
}

UISizer* uiWindow::guiSizer(void)
{
	UISizer *sizer = create_sizer();

	return sizer;
}


void uiWindow::InsertTextChild(TiXmlElement* element, const char *name, const char *value)
{
	element->InsertEndChild(TiXmlElement(name));
	TiXmlElement *child = element->FirstChildElement(name);

	TiXmlText *text = new TiXmlText(value);
	child->LinkEndChild(text);
}

TiXmlElement* uiWindow::GetXMLElement(void)
{
	TiXmlElement *element = new TiXmlElement("object");

	element->SetAttribute("class",		m_Properties.find(PROP_CLASS)->second->xmlString());
	element->SetAttribute("name",		m_Properties.find(PROP_NAME)->second->xmlString());
	element->SetAttribute("variable",	m_Properties.find(PROP_VAR_NAME)->second->xmlString());
	element->SetAttribute("backColor",	m_Properties.find(PROP_BKG_COLOR)->second->xmlString());

	element->InsertEndChild(TiXmlElement("rect"));
	TiXmlElement *rectElement = element->FirstChildElement("rect");
	PropertyRect *position = (PropertyRect *)m_Properties.find(PROP_POSITION)->second;
	rectElement->SetAttribute("x", position->GetX());
	rectElement->SetAttribute("y", position->GetY());
	rectElement->SetAttribute("width", position->GetWidth());
	rectElement->SetAttribute("height", position->GetHeight());

	return element;
}

void uiWindow::SetProperties(const TiXmlElement* element)
{
	if (element == nullptr)
		return;

	const char *value = element->Attribute("name");
    if (dsC2U(value).empty())
    {
        wxLogMessage("Name is null!",element->Attribute("name"));
    }
    else
    {
       SetStringProperty(PROP_NAME, value);
    }

	value = element->Attribute("variable");
    if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no variable property!",element->Attribute("name"));
    }
    else
    {
        SetStringProperty(PROP_VAR_NAME, value);
    }

	value = element->Attribute("backColor");
    if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no image property!",element->Attribute("name"));
    }
    else
    {
        SetColorProperty(PROP_BKG_COLOR, value);
    }

	wxRect rectValue;
	const TiXmlElement *rect = element->FirstChildElement("rect");
	if (rect)
	{
		rect->QueryIntAttribute("x", &(rectValue.x));
		rect->QueryIntAttribute("y", &(rectValue.y));
		rect->QueryIntAttribute("width", &(rectValue.width));
		rect->QueryIntAttribute("height", &(rectValue.height));
	}
	SetProperty(PROP_POSITION, rectValue);
}


void uiWindow::SetProperty(const wxString& key, const wxAny& value)
{
	PropertyBase *prop = m_Properties.find(key)->second;

	if (!prop) return;		/* only set existing property */

	if (prop->m_Type == PropertyType_Integer)
	{
		PropertyInteger *intProperty = (PropertyInteger *)prop;
		intProperty->SetIntValue(value.As<int>());
	}
	else if (prop->m_Type == PropertyType_Enum)
    {
        PropertyEnum *enumProperty = (PropertyEnum *)prop;
		enumProperty->SetEnumValue(value.As<int>());
    }
	else if (prop->m_Type == PropertyType_Bool)
    {
        PropertyBool *boolProperty = (PropertyBool *)prop;
		boolProperty->SetBoolValue(value.As<bool>());
    }
    else if (prop->m_Type == PropertyType_InternalString)
	{
		PropertyInternalString *stringProperty = (PropertyInternalString *)prop;
		wxString StringValue = value.As<wxString>();

		if (!Manager::Get()->GetActiveProject() || !StringManager::Get()->m_Stringpaths.IsEmpty())
		{
			if (value.CheckType<int>())
			{
				stringProperty->SetStringValue(StringManager::Get()->GetString(value.As<int>(), true));
				stringProperty->SetSaveIdValue(value.As<int>());
			}
			else
			{
				stringProperty->SetStringValue(value.As<wxString>());
				stringProperty->SetSaveIdValue(-2);
			}
		}
		else
		{
			stringProperty->SetStringValue(value.As<wxString>());
			stringProperty->SetSaveIdValue(-2);
		}
	}
	else if (prop->m_Type == PropertyType_String)
	{
		PropertyString *stringProperty = (PropertyString *)prop;
		stringProperty->SetStringValue(value.As<wxString>());
	}
	else if (prop->m_Type == PropertyType_Fold)
	{
		PropertyFold *stringProperty = (PropertyFold *)prop;
		stringProperty->SetStringValue(value.As<wxString>());
	}
	else if (prop->m_Type == PropertyType_Rect)
	{
		PropertyRect *rectProperty = (PropertyRect *)prop;
		rectProperty->SetRect(value.As<wxRect>());
	}
	else if (prop->m_Type == PropertyType_Size)
	{
		PropertySize *sizeProperty = (PropertySize *)prop;
		sizeProperty->SetSize(value.As<wxSize>());
	}
	else if (prop->m_Type == PropertyType_Color)
	{
		PropertyColor *colorProperty = (PropertyColor *)prop;
		wxColourPropertyValue cpv = value.As<wxColourPropertyValue>();
		colorProperty->SetColor(cpv.m_type, cpv.m_colour);
	}
	else if (prop->m_Type == PropertyType_Image)
	{
		PropertyImage *imageProperty = (PropertyImage *)prop;
		long imageValue = value.As<long>();

		if (imageValue != 0xFFFFFF && Manager::Get()->GetActiveProject())
		{
			imageProperty->SetImage(Manager::Get()->GetActiveProject()->m_Images.GetImageName(imageValue));
		}
	}
	else if (prop->m_Type == PropertyType_Logo)
    {
        PropertyLogo *logoProperty = (PropertyLogo *)prop;
		long LogoValue = value.As<long>();

		if (LogoValue != 0xFFFFFF && LogoValue != -1 && Manager::Get()->GetActiveProject())
		{
			logoProperty->SetLogoIndex(LogoValue);
			logoProperty->SetLogo(Manager::Get()->GetActiveProject()->m_LogoImage.GetImageName(LogoValue));
			logoProperty->SetLogoFileName(Manager::Get()->GetActiveProject()->m_LogoImage.GetImageFileName(LogoValue));
		}
		else
		{
			logoProperty->SetLogoIndex(-1);
		}
    }

}


void uiWindow::SetIntegerProperty(const wxString& key, const char *value)
{
	SetProperty(key, PropertyInteger(value).IntValue());
}

void uiWindow::SetStringProperty(const wxString& key, const char *value)
{
	wxString sv(value);
	SetProperty(key, sv);
}

void uiWindow::SetColorProperty(const wxString& key, const char* value, unsigned int defaultValue)
{
	PropertyColor color(value, defaultValue);
	wxColourPropertyValue cpv(Colors::Get()->GetColorId(color.Name()), color.Value());

	SetProperty(key, cpv);
}

void uiWindow::SetColorProperty(const wxString& key, const wxColour& value)
{
	wxColourPropertyValue cpv(value);
	SetProperty(key, cpv);
}

void uiWindow::SetImageProperty(const wxString& key, const char *value)
{
	PropertyBase *prop = m_Properties.find(key)->second;
	if (!prop)	/* only set existing property */
		return;

	if (prop->m_Type == PropertyType_Image)
	{
		wxString iv(value);
		PropertyImage *imageProperty = (PropertyImage *)prop;
		imageProperty->SetImage(iv);
	}
}

void uiWindow::SetLogoProperty(const wxString& key, const char *value)
{
	PropertyBase *prop = m_Properties.find(key)->second;
	if (!prop)	/* only set existing property */
		return;

	if (prop->m_Type == PropertyType_Logo)
	{
		wxString iv(value);
		PropertyLogo *LogoProperty = (PropertyLogo *)prop;
		LogoProperty->SetLogo(iv);
	}
}

void uiWindow::SetBoolProperty(const wxString& key, const char *value)
{
	SetProperty(key, PropertyBool(atoi(value)).BoolValue());
}

void uiWindow::SetEnumProperty(const wxString& key, const char *value)
{
	SetProperty(key, PropertyEnum(value).EnumValue());
}

void uiWindow::RefreshString(void)
{
	Linked_map<wxString, PropertyBase*>::iterator it;
	for (it = m_Properties.begin(); it != m_Properties.end(); ++it)
	{
		wxString key = it->first;
		PropertyBase *prop = it->second;

		if (prop && prop->m_Type == PropertyType_InternalString )
		{
			PropertyInternalString *StringProperty = (PropertyInternalString *)prop;
			if (StringProperty->GetSaveIdValue() != -2)
			{
				wxAny value = StringProperty->GetSaveIdValue();
				SetProperty(key, value);
			}
		}
	}
}


void uiWindow::RefreshColor(void)
{
	Linked_map<wxString, PropertyBase*>::iterator it;
	for (it = m_Properties.begin(); it != m_Properties.end(); ++it)
	{
		wxString key = it->first;
		PropertyBase *prop = it->second;

		if (prop && prop->m_Type == PropertyType_Color)
		{
			PropertyColor *colorProperty = (PropertyColor *)prop;
			unsigned int id = Colors::Get()->GetColorId(colorProperty->Name());
			wxColour value;

			if (id != Colors::CustomColorId())
			{
				/* get latest color value */
				value = Colors::Get()->GetColor(id);

				wxColourPropertyValue cpv(id, value);
				SetProperty(key, cpv);
			}
		}
	}
}


uiWindow* uiWindow::FindInSubWindow(wxString& Id)
{
	return FindInSubWindowRecursively(this, Id);
}


void uiWindow::AddSubWindow(uiWindow *addThis)
{
	if (addThis == nullptr)
		return;

	addThis->m_Parent = this;
	if (this->m_Childs)
	{
		uiWindow *brother = this->m_Childs;

		while (brother->m_Next != nullptr)
			brother = brother->m_Next;

		brother->m_Next = addThis;
	}
	else
	{
		this->m_Childs = addThis;
	}
}

void uiWindow::MoveUpSubWindow(uiWindow *upThis)
{
	if ((upThis == nullptr) || (upThis->m_Parent == nullptr))
	{
		return;
	}

	uiWindow *first = upThis->m_Parent->m_Childs;
	if(first->Id() == upThis->Id())
	{
		return;
	}

	uiWindow *second = first->m_Next;
	if (second != nullptr )
	{
		if(second->Id() == upThis->Id())
		{
			upThis->m_Parent->m_Childs = second;
			first->m_Next = second->m_Next;
			second->m_Next = first;
			return;
		}

		uiWindow *three = second->m_Next;
		while (three != nullptr)
		{
			if (three->Id() == upThis->Id())
			{
				first->m_Next = three;
				second->m_Next = three->m_Next;
				three->m_Next = second;
				return;
			}
			first = second;
			second = three;
			three = three->m_Next;
		}
	}
}

void uiWindow::MoveDownSubWindow(uiWindow *downThis)
{
	if ((downThis == nullptr) || (downThis->m_Parent == nullptr))
	{
		return;
	}

	uiWindow *first = downThis->m_Parent->m_Childs;
	uiWindow *second = first->m_Next;
	if(second != nullptr)
	{
		if (first->Id() == downThis->Id())
		{
			downThis->m_Parent->m_Childs = second;
			first->m_Next = second->m_Next;
			second->m_Next = first;
			return;
		}

		uiWindow *three = second->m_Next;
		while (three != nullptr)
		{
			if (second->Id() == downThis->Id())
			{
				first->m_Next = three;
				second->m_Next = three->m_Next;
				three->m_Next = second;
				return;
			}
			first = second;
			second = three;
			three = three->m_Next;
		}
	}
}

void uiWindow::DeleteSubWindow(uiWindow *deleteThis)
{
	if ((deleteThis == nullptr) || (deleteThis->m_Parent == nullptr))
	{
		return;
	}

	uiWindow *first = deleteThis->m_Parent->m_Childs;
	uiWindow *second = first->m_Next;
	if (first != nullptr )
	{
		if(first->Id() == deleteThis->Id())
		{
			first->m_Parent->m_Childs = second;
			first->m_Next = nullptr;
		}
		else
		{
			while (second != nullptr)
			{
				if (second->Id() == deleteThis->Id())
				{
					first->m_Next = second->m_Next;
					second->m_Next = nullptr;
					break;
				}
				first = second;
				second = second->m_Next;
			}
		}

		DeleteSubWindowRecursively(deleteThis);
	}
}


int uiWindow::GetIntPropValue(wxString key, int defaultValue)
{
	PropertyBase *prop = m_Properties.find(key)->second;

	if (prop && prop->m_Type == PropertyType_Integer)
	{
		return ((PropertyInteger *)prop)->IntValue();
	}
	return defaultValue;
}

bool uiWindow::GetBoolPropValue(wxString key, int defaultValue)
{
	PropertyBase *prop = m_Properties.find(key)->second;

	if (prop && prop->m_Type == PropertyType_Bool)
	{
		return ((PropertyBool *)prop)->BoolValue();
	}
	return defaultValue;
}

int uiWindow::GetEnumPropValue(wxString key, int defaultValue)
{
	PropertyBase *prop = m_Properties.find(key)->second;

	if (prop && prop->m_Type == PropertyType_Enum)
	{
		return ((PropertyEnum *)prop)->EnumValue();
	}
	return defaultValue;
}

const char* uiWindow::GetStrPropValue(wxString key, const char* defaultValue)
{
	PropertyBase *prop = m_Properties.find(key)->second;

	if (prop && prop->m_Type == PropertyType_String)
	{
		return ((PropertyString *)prop)->xmlString();
	}
	else if (prop && prop->m_Type == PropertyType_InternalString)
	{
		/*check internal string if exist*/
		PropertyInternalString *stringProperty = (PropertyInternalString *)prop;
		bool IsExistInternalString = false;
		wxString PropStr(stringProperty->GetStringValue());
		if (stringProperty->GetSaveIdValue() != -2 && stringProperty->GetSaveIdValue() != -1)
		{
			for (size_t i = 0; i < str_max_strings; i++)
			{
				if (StringManager::Get()->GetString(i, true) == PropStr)
				{
					IsExistInternalString = true;
					break;
				}
			}
		}
		else
		{
			IsExistInternalString = true;
		}
		if (IsExistInternalString == false)
		{
			wxLogMessage(wxT("uiLabel字符串引用错误：(%s) 内部字符串不存在！\n"), PropStr);
		}

		return ((PropertyInternalString *)prop)->GetStringValue();
	}
	else if (prop && prop->m_Type == PropertyType_Fold)
	{
		return ((PropertyFold *)prop)->xmlString();
	}
	return defaultValue;
}

wxRect uiWindow::GetRectPropValue(wxString key, wxRect defaultValue)
{
	PropertyBase *prop = m_Properties.find(key)->second;

	if (prop && prop->m_Type == PropertyType_Rect)
	{
		return ((PropertyRect *)prop)->Value();
	}
	return defaultValue;
}

wxSize uiWindow::GetSizePropValue(wxString key, wxSize defaultValue)
{
	PropertyBase *prop = m_Properties.find(key)->second;

	if (prop && prop->m_Type == PropertyType_Size)
	{
		return ((PropertySize *)prop)->Value();
	}
	return defaultValue;
}

U32 uiWindow::GetColorPropValue(wxString key, U32 defaultValue)
{
	PropertyBase *prop = m_Properties.find(key)->second;

	if (prop && prop->m_Type == PropertyType_Color)
	{
		PropertyColor *propColor = (PropertyColor *)prop;
		bool IsExistColor = false;
		if (!propColor->Name().empty())
		{
			if (Colors::Get()->GetColorId(propColor->Name()) != Colors::CustomColorId())
			{
				IsExistColor = true;
			}
		}
		else
		{
			IsExistColor = true;
		}

		if (IsExistColor == false)
		{
			wxLogMessage(wxT("颜色引用出错：(%s) 颜色不存在\n"),propColor->Name());
		}

		return ((PropertyColor *)prop)->UIntValue();
	}
	return defaultValue;
}

const char* uiWindow::GetImagePropValue(wxString key)
{
	PropertyBase *prop = m_Properties.find(key)->second;

	if (prop && prop->m_Type == PropertyType_Image)
	{
		/*check the image if exist*/
		PropertyImage *propImage = (PropertyImage *)prop;
		wxString ImageName(propImage->xmlString());
		bool IsImageExist = false;
		if (!ImageName.empty())
		{
			for(size_t i = 0; i < Manager::Get()->GetActiveProject()->m_Images.Count(); i++)
			{
				if (ImageName == Manager::Get()->GetActiveProject()->m_Images.GetImageAt(i)->GetName())
				{
					IsImageExist = true;
					break;
				}
			}
		}
		if (IsImageExist == false)
		{
			wxLogMessage(wxT("uiImage图片引用错误: %s 图片不存在！\n"), ImageName);
		}
		return ((PropertyImage *)prop)->xmlString();
	}
	return "";
}


int uiWindow::GetLogoIndexPropValue(wxString key)
{
	PropertyBase *prop = m_Properties.find(key)->second;

	if (prop && prop->m_Type == PropertyType_Logo)
	{
		return ((PropertyLogo *)prop)->LogoIndex();
	}
	return -1;
}


wxString uiWindow::RandomId(void)
{
	char id[32];
	int i, rndNumber;

	for (i = 0; i < 23; i++)
	{
		if ((i + 1) % 8 == 0)
		{
			id[i] = '_';
		}
		else
		{
			rndNumber = rand() % 52;
			id[i] = rndNumber < 26 ? ('A' + rndNumber) : ('a' + (rndNumber - 26));
		}
	}
	id[23] = '\0';

	return wxString(id);
}

uiWindow* uiWindow::FindInSubWindowRecursively(uiWindow *current, wxString& Id)
{
	if (current == nullptr) return nullptr;

	if (current->Id() == Id) return current;

	uiWindow *window, *childs;
	childs = current->m_Childs;
	while (childs)
	{
		window = FindInSubWindowRecursively(childs, Id);
		if (window)
			return window;

		childs = childs->m_Next;
	}

	return nullptr;
}

void uiWindow::DeleteSubWindowRecursively(uiWindow *current)
{
	if (current == nullptr) return;

	uiWindow *childs, *nextChilds;
	childs = current->m_Childs;

	while (childs)
	{
	    nextChilds = childs->m_Next;
		DeleteSubWindowRecursively(childs);
		childs = nextChilds;
	}

	delete current;
}


void uiWindow::RepositionSubWindowRecursively(uiWindow* parent)
{
    wxRect rect;

    uiWindow* sub_win = parent->m_Childs;
    while (sub_win)
	{
//	    wxLogMessage("parent = %s  sub = %s", parent->Id(), sub_win->Id());
		if ((sub_win->stb_window) || (sub_win->stb_sizer))
        {
            if ((sub_win->GetWindowType() == WINDOW_TYPE_SIZER) && (sub_win->stb_sizer))
            {
//                wxLogMessage("add sizer = %s", sub_win->Id());
                RepositionSubWindowRecursively(sub_win);
                rect.x = sub_win->stb_sizer->position.x;
                rect.y =  sub_win->stb_sizer->position.y;
                rect.width =  sub_win->stb_sizer->size.width;
                rect.height =  sub_win->stb_sizer->size.height;
                rect.x +=  parent->m_absolute_pos.x;
                rect.y +=  parent->m_absolute_pos.y;
            }
            else if (sub_win->stb_window)
            {
//                wxLogMessage("add win = %s", sub_win->Id());
                rect.x = sub_win->stb_window->rect.point.x;
                rect.y =  sub_win->stb_window->rect.point.y;
                rect.width =  sub_win->stb_window->rect.size.width;
                rect.height =  sub_win->stb_window->rect.size.height;
                wxLogMessage("stb_window id=%s x=%d,y=%d,w=%d,h=%d",sub_win->stb_window->id, rect.x, rect.y, rect.width, rect.height);
            }

//            wxLogMessage("id=%s x=%d,y=%d,w=%d,h=%d",sub_win->Id(), rect.x, rect.y, rect.width, rect.height);
            sub_win->SetProperty(PROP_POSITION, rect);
        }


		sub_win = sub_win->m_Next;
	}
}


void uiWindow::OnBuildDeclarationsCode()
{
	AddDeclaration(wxString::Format(wxT("UIWindow *%s = create_window(\"%s\");"), GetStrPropValue(PROP_VAR_NAME), GetName()));
}

void uiWindow::OnBuildWindowStructureCode()
{
	if (m_Parent)
		AddStructure(wxString::Format(wxT("add_sub_window(%s, %s);"), m_Parent->GetStrPropValue(PROP_VAR_NAME), GetStrPropValue(PROP_VAR_NAME)));
}

void uiWindow::OnBuildCreatingCode()
{
	AddBuildingCode(wxString::Format(wxT("set_window_color(%s, 0x%08X);\n"), GetStrPropValue(PROP_VAR_NAME), GetColorPropValue(PROP_BKG_COLOR)));
}

void uiWindow::OnBuildIdCode()
{
//	AddIdCode();
}

wxPGChoices uiWindow::AddEnmuChildProperty(wxString key)
{
	wxPGChoices choice;

	return choice;
}

void uiWindow::AddUpdateChildProperty(const TiXmlElement* element)
{

}



//-------------------------------------------------------------------------------------------------
// window factory
//-------------------------------------------------------------------------------------------------
int WindowFactory::ClassId(const char *className)
{
	const char* supportedClass[] =
	{
			"UIPage", 	"UILabel",		"UIImage",	"UIList",
			"UIEdit",	"UIEditItem",	"UIMenu",	"UIMenuItem",
			"UIHelp",	"UILineChart",	"UIPrompt",	"UIMessageBox",
			"UITextField", "UIListSizer",	"UIBoxSizer"
	};

	for (unsigned int i = 0; i < sizeof(supportedClass) / sizeof(supportedClass[0]); i++)
	{
		if (strcmp(className, supportedClass[i]) == 0)
			return i;
	}
	return -1;
}

uiWindow* WindowFactory::Create(const char *className)
{
	if (!className || strlen(className) <= 0)
		return nullptr;

	int classId = ClassId(className);

	switch (classId)
	{
		case 0:
			return new uiPage();

		case 1:
			return new uiLabel();

		case 2:
			return new uiImage();

		case 3:
			return new uiList();

		case 4:
			return new uiEdit();

		case 5:
			return new uiEditItem();

		case 6:
			return new uiMenu();

		case 7:
			return new uiMenuItem();

		case 9:
			return new uiLineChart();

		case 10:
			return new uiPrompt();

		case 11:
			return new uiMessageBox();

		case 12:
			return new uiTextField();

		case 13:
			return new uiListSizer();

		case 14:
			return new uiBoxSizer();

		default:
			break;
	}
	return new uiWindow();
}
