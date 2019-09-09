/*
 * uiimage.cpp
 *
 *  Created on: 2019年4月25日
 *      Author: Brant
 */
#include "manager.h"
#include "window/uiimage.h"
using namespace std;

uiImage::uiImage(uiWindow *parent) : uiWindow(parent)
{
	/* override property */
	SetStringProperty(PROP_CLASS,		"UIImage");
	m_Properties.find(PROP_BKG_COLOR)->second->m_Visiable = false;
	/* specially for UILabel */
	m_Properties.insert(pair<wxString, PropertyBase*>("Image Property", new PropertyFold("Image Property")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_IMAGE_NAME, new PropertyImage()));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_AUTO_SIZE, new PropertyBool(TRUE)));

	/* Marked as a control */
	m_type = WINDOW_TYPE_CONTROL;
	m_icon_index = ICON_CONTROL_IMAGE;
}


uiImage::~uiImage(void)
{

}

wxPGChoices uiImage::AddEnmuChildProperty(wxString key)
{
	wxPGChoices choice;

	return choice;
}

UIWindow* uiImage::guiWindow(void)
{
	UIWindow *window = create_image(GetStrPropValue(PROP_NAME));
    set_image_auto_size(window,GetBoolPropValue(PROP_AUTO_SIZE));
	wxRect rect = GetRectPropValue(PROP_POSITION);
	set_window_rect(window, ui_rect(rect.x, rect.y, rect.width, rect.height));
	set_built_in_image(window, GetImagePropValue(PROP_IMAGE_NAME));

	return window;
}


TiXmlElement* uiImage::GetXMLElement(void)
{
	TiXmlElement* element = uiWindow::GetXMLElement();

	/* append UIImage property */
	element->SetAttribute("image", m_Properties.find(PROP_IMAGE_NAME)->second->xmlString());
	//element->SetAttribute("Auto Size", m_Properties.find(PROP_AUTO_SIZE)->second->xmlString());

	return element;
}


void uiImage::SetProperties(const TiXmlElement* element)
{
	/* read base property */
	uiWindow::SetProperties(element);

	/* read UIImage special property */
	const char *value = element->Attribute("image");
    if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no image property!",element->Attribute("name"));
    }
    else
    {
        SetImageProperty(PROP_IMAGE_NAME, value);
    }
//	value = element->Attribute("Auto Size");
//	if(value)
//		SetImageAutoSizeProperty(PROP_AUTO_SIZE,value);

}

void uiImage::OnBuildDeclarationsCode()
{
	AddDeclaration(wxString::Format(wxT("UIImage *%s = create_image(\"%s\");"), GetStrPropValue(PROP_VAR_NAME), GetName()));
}

void uiImage::OnBuildCreatingCode()
{
//	uiWindow::OnBuildCreatingCode();
	AddBuildingCode(wxString::Format(wxT("set_built_in_image(%s, \"%s\");\n"), GetStrPropValue(PROP_VAR_NAME), GetImagePropValue(PROP_IMAGE_NAME)));
	AddBuildingCode("\n");
}

void uiImage::OnBuildIdCode()
{

}
