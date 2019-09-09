/*
 * uibox_sizer.cpp
 *
 *  Created on: 2019-8-14
 *      Author: Antony
 */

#include "window/uibox_sizer.h"


uiBoxSizer::uiBoxSizer()
{
	/* override property */
	SetStringProperty(PROP_CLASS,		"UIBoxSizer");
	m_Properties.find(PROP_BKG_COLOR)->second->m_Visiable = false;

	/* specially for UIListSizer */
	m_Properties.insert(pair<wxString, PropertyBase*>("BoxSizer Property", new PropertyFold("BoxSizer Property")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_DIRECTION, new PropertyEnum(1)));

	m_type = WINDOW_TYPE_SIZER;
	m_icon_index = ICON_CONTROL_BOX_SIZER;
}


uiBoxSizer::~uiBoxSizer(void)
{

}

wxPGChoices uiBoxSizer::AddEnmuChildProperty(wxString key)
{
	wxPGChoices choice;
	if (key.IsSameAs(PROP_DIRECTION))
	{
		choice.Add("HORIZONTAL", 0);
		choice.Add("VERTICAL", 1);
	}
	return choice;
}

UISizer* uiBoxSizer::guiSizer(void)
{
	UISizer *sub_sizer = create_box_sizer(GetEnumPropValue(PROP_DIRECTION));

    if (this->m_Parent->GetWindowType() == WINDOW_TYPE_SIZER)
    {
        this->stb_sizer = sub_sizer;
        UISizer *parent_sizer = this->m_Parent->stb_sizer;
        if (parent_sizer)
        {
            sizer_add_sub_sizer(parent_sizer, sub_sizer, 1, 5, UI_SIZER_FLAG_BORDER_TOP|UI_SIZER_FLAG_BORDER_LEFT|UI_ALIGN_CENTER_VER);
            uiWindow *parent = this->m_Parent->m_Parent;
            while(parent)
            {
                if (parent->GetWindowType() == WINDOW_TYPE_SUB)
                {
                    if (parent->stb_window)
                    {
                        set_container_sizer(parent->stb_window, sub_sizer);
                    }
                    break;
                }
                parent = parent->m_Parent;
            }
            this->RepositionSubWindowRecursively(parent);
        }
    }
    else if(this->m_Parent->GetWindowType() == WINDOW_TYPE_SUB)
    {
        UIWindow *parent_window = this->m_Parent->stb_window;
//        wxLogMessage("parent_window--->%s",parent_window->id);
        if (parent_window)
        {
            set_container_sizer(parent_window, sub_sizer);
        }
        this->m_absolute_pos.x = parent_window->rect.point.x + sub_sizer->position.x;
        this->m_absolute_pos.y = parent_window->rect.point.y + sub_sizer->position.y;
//        wxLogMessage("x=%d,y=%d,w=%d,h=%d",sub_sizer->position.x, sub_sizer->position.y, sub_sizer->size.width, sub_sizer->size.height);

        this->SetProperty(PROP_POSITION, wxRect(this->m_absolute_pos.x, this->m_absolute_pos.y, sub_sizer->size.width, sub_sizer->size.height));
    }

	return sub_sizer;
}


TiXmlElement* uiBoxSizer::GetXMLElement(void)
{
	TiXmlElement* element = uiWindow::GetXMLElement();

	element->SetAttribute("direction", 			m_Properties.find(PROP_DIRECTION)->second->xmlString());
	return element;
}


void uiBoxSizer::SetProperties(const TiXmlElement* element)
{
	/* read base property */
	uiWindow::SetProperties(element);

	/* read UIListSizer special property */
	const char* value = element->Attribute("direction");
    if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no direction property!",element->Attribute("name"));
    }
    else
    {
    	SetEnumProperty(PROP_DIRECTION, value);
    }

}


void uiBoxSizer::OnBuildDeclarationsCode()
{

}

void uiBoxSizer::OnBuildCreatingCode()
{
	uiWindow::OnBuildCreatingCode();
	AddBuildingCode("\n");
}

void uiBoxSizer::OnBuildIdCode()
{

}





