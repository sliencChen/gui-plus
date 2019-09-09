/*
 * uimessage_box.cpp
 *
 *  Created on: 2019-8-11
 *      Author: Antony
 */

#include "window/uimessage_box.h"


uiMessageBox::uiMessageBox(uiWindow *parent) : uiWindow(parent)
{
	/* override property */
	SetStringProperty(PROP_CLASS,		"UIMessageBox");
	SetProperty(PROP_POSITION, wxRect(0, 0, MESSAGE_BOX_DEFAULT_WIDTH, MESSAGE_BOX_DEFAULT_HEIGHT));
	m_Properties.find(PROP_BKG_COLOR)->second->m_Visiable = false;
	/* specially for UIPrompt */
	m_Properties.insert(pair<wxString, PropertyBase*>("MessageBox Property", new PropertyFold("MessageBox Property")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_TITLE_STRING, new PropertyInternalString("Null value")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_CONTENT, new PropertyInternalString("Null value")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_MESSAGE_BOX_TYPE, new PropertyEnum(1)));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_DIGIT_NUM, new PropertyInteger(4)));


	/* Marked as a control */
	m_type = WINDOW_TYPE_SUB;
	m_icon_index = ICON_CONTROL_MESSAGE_BOX;
}


uiMessageBox::~uiMessageBox(void)
{

}

wxPGChoices uiMessageBox::AddEnmuChildProperty(wxString key)
{
	wxPGChoices choice;
	if (key.IsSameAs(PROP_MESSAGE_BOX_TYPE))
	{
		choice.Add("SELECT_OPTION_TYPE", 0);
		choice.Add("INPUT_NUMERIC_TYPE", 1);
	}
	return choice;
}

UIWindow* uiMessageBox::guiWindow(void)
{
	UIWindow *window =  create_message_box(GetStrPropValue(PROP_NAME));

	set_message_box_title(window, GetStrPropValue(PROP_TITLE_STRING));
	set_message_box_content(window, GetStrPropValue(PROP_CONTENT));
	set_message_action_type(window, GetEnumPropValue(PROP_MESSAGE_BOX_TYPE));
	if (GetEnumPropValue(PROP_MESSAGE_BOX_TYPE) == true)
	{
		m_Properties.find(PROP_DIGIT_NUM)->second->m_Visiable = true;
		set_message_box_digit_num(window, GetIntPropValue(PROP_DIGIT_NUM));
	}
	else
	{
		m_Properties.find(PROP_DIGIT_NUM)->second->m_Visiable = false;
	}

	wxRect rect = GetRectPropValue(PROP_POSITION);
	set_window_rect(window, ui_rect(rect.x, rect.y, rect.width, rect.height));

	m_absolute_pos.x = rect.x;
	m_absolute_pos.y = rect.y;
	return window;
}


TiXmlElement* uiMessageBox::GetXMLElement(void)
{
	TiXmlElement* element = uiWindow::GetXMLElement();

	InsertTextChild(element, "title", 	m_Properties.find(PROP_TITLE_STRING)->second->xmlString());
	InsertTextChild(element, "content", m_Properties.find(PROP_CONTENT)->second->xmlString());
	element->SetAttribute("supportInput", 		m_Properties.find(PROP_MESSAGE_BOX_TYPE)->second->xmlString());
	element->SetAttribute("digitNum", 		m_Properties.find(PROP_DIGIT_NUM)->second->xmlString());

	return element;
}


void uiMessageBox::SetProperties(const TiXmlElement* element)
{
	/* read base property */
	uiWindow::SetProperties(element);

	/* read UIPrompt special property */
	const TiXmlElement *text = element->FirstChildElement("title");
	if (dsC2U(text->GetText()).empty())
    {
        wxLogMessage("%s have no title property!",element->Attribute("name"));
    }
    else
    {
        SetStringProperty(PROP_TITLE_STRING, text->GetText());
    }

	text = element->FirstChildElement("content");
	if (dsC2U(text->GetText()).empty())
    {
        wxLogMessage("%s have no content property!",element->Attribute("name"));
    }
    else
    {
        SetStringProperty(PROP_CONTENT, text->GetText());
    }

	const char *value = element->Attribute("supportInput");
	if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no supportInput property!",element->Attribute("name"));
    }
    else
    {
        SetEnumProperty(PROP_MESSAGE_BOX_TYPE, value);
    }

	value = element->Attribute("digitNum");
	if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no digitNum property!",element->Attribute("name"));
    }
    else
    {
        SetIntegerProperty(PROP_DIGIT_NUM, value);
    }

}


void uiMessageBox::OnBuildDeclarationsCode()
{

	AddDeclaration(wxString::Format(wxT("UIMessageBox *%s = create_message_box(\"%s\");"), GetStrPropValue(PROP_VAR_NAME), GetName()));
}

void uiMessageBox::OnBuildCreatingCode()
{
	uiWindow::OnBuildCreatingCode();
	AddBuildingCode(wxString::Format(wxT("set_message_box_title(%s, \"%d\");\n"), GetStrPropValue(PROP_VAR_NAME), GetStrPropValue(PROP_TITLE_STRING)));
	AddBuildingCode(wxString::Format(wxT("set_message_box_content(%s, \"%d\");\n"), GetStrPropValue(PROP_VAR_NAME), GetStrPropValue(PROP_CONTENT)));
	AddBuildingCode(wxString::Format(wxT("set_message_action_type(%s, \"%d\");\n"), GetStrPropValue(PROP_VAR_NAME), GetBoolPropValue(PROP_MESSAGE_BOX_TYPE)));
	if (GetBoolPropValue(PROP_MESSAGE_BOX_TYPE) == true)
	{
		AddBuildingCode(wxString::Format(wxT("set_message_box_digit_num(%s, \"%d\");\n"), GetStrPropValue(PROP_VAR_NAME), GetIntPropValue(PROP_DIGIT_NUM)));
	}

	AddBuildingCode("\n");
}

void uiMessageBox::OnBuildIdCode()
{

}





