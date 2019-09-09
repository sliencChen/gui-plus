/*
 * uiline.cpp
 *
 *  Created on: 2019-8-2
 *      Author: Antony
 */

#include "window/uiline_chart.h"

uiLineChart::uiLineChart(uiWindow *parent) : uiWindow(parent)
{
	/* override property */
	SetStringProperty(PROP_CLASS,		"UILineChart");
	SetColorProperty(PROP_BKG_COLOR,	wxColour(0, 0, 0, 0));
	SetProperty(PROP_POSITION, wxRect(0, 0, LINE_CHART_DEFAULT_WIDTH, LINE_CHART_DEFAULT_HEIGHT));

	/* specially for UILabel */
	m_Properties.insert(pair<wxString, PropertyBase*>("LineChart Property", new PropertyFold("LineChart Property")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_TOTAL_NUM, new PropertyInteger(3)));

	/* Marked as a control */
	m_type = WINDOW_TYPE_CONTROL;
	m_icon_index = ICON_CONTROL_LINE_CHART;
}


uiLineChart::~uiLineChart(void)
{

}

wxPGChoices uiLineChart::AddEnmuChildProperty(wxString key)
{
	wxPGChoices choice;

	return choice;
}

void uiLineChart::AddUpdateChildProperty(const TiXmlElement* element)
{
	int save_index = 0;
	U32 bmp_num[MAX_LINE_CHART_NUM] = {0};
	U32 save_line_color[MAX_LINE_CHART_NUM] = {0};
	U32 save_point[MAX_LINE_CHART_NUM] = {0};
	U32 save_point_color[MAX_LINE_CHART_NUM] = {0};
	U32 save_text_color[MAX_LINE_CHART_NUM] = {0};
	char *save_bmp[MAX_POINT_ARRAY_NUM * MAX_LINE_CHART_NUM] = {NULL};
	int save_point_pos[MAX_LINE_CHART_NUM][MAX_POINT_ARRAY_NUM] = {0};
	int bmp_index = 0;
	if (save_line_chart_num > MAX_LINE_CHART_NUM)
	{
		return;
	}

	/* if first open window, must input some necessary init value */
	if (first_open_window == true)
	{
		first_open_window = false;
		save_line_chart_num = GetIntPropValue(PROP_TOTAL_NUM);
		if (element)
		{
			int i = 0;
			const TiXmlElement *linechartInfo = element->FirstChildElement("lineChart");
			const TiXmlElement *Info = linechartInfo->FirstChildElement("Info");
			while (Info)
			{
				if (atoi(Info->Attribute("point_num")) > 0)
					save_point[i] = atoi(Info->Attribute("point_num"));
				else
					save_point[i] = 5;
				Info = Info->NextSiblingElement("Info");
				i++;
			}
		}
		else
		{
			for (int index = 0; index < save_line_chart_num; index++)
			{
				save_point[index] = 5;
			}
		}
	}

	/* save the value before erase data */
	if (save_line_chart_num != 0)
	{
		bmp_index = 0;
		for (int i = 0; i < save_line_chart_num; i++)
		{
			if (m_Properties.find(wxString::Format("line_chart_%d", i)) != m_Properties.end())
			{
				m_Properties.erase(m_Properties.find(wxString::Format("line_chart_%d", i)));
			}
			if (m_Properties.find(wxString::Format("line_color_%d", i)) != m_Properties.end())
			{
				save_line_color[i] = GetColorPropValue(wxString::Format("line_color_%d", i));
				m_Properties.erase(m_Properties.find(wxString::Format("line_color_%d", i)));
			}
			if (m_Properties.find(wxString::Format("point_color_%d", i)) != m_Properties.end())
			{
				save_point_color[i] = GetColorPropValue(wxString::Format("point_color_%d", i));
				m_Properties.erase(m_Properties.find(wxString::Format("point_color_%d", i)));
			}
			if (m_Properties.find(wxString::Format("text_color_%d", i)) != m_Properties.end())
			{
				save_text_color[i] = GetColorPropValue(wxString::Format("text_color_%d", i));
				m_Properties.erase(m_Properties.find(wxString::Format("text_color_%d", i)));
			}
			if (m_Properties.find(wxString::Format("point_num_%d", i)) != m_Properties.end())
			{
				save_point[i] = GetIntPropValue(wxString::Format("point_num_%d", i));
				for (int j = 0; j < save_point_num[i]; j++)
				{
					if (m_Properties.find(wxString::Format("pos%d_%d", i,j)) != m_Properties.end())
					{
						save_point_pos[i][j] = GetIntPropValue(wxString::Format("pos%d_%d", i,j));
						save_bmp[bmp_index] = (char *)GetImagePropValue(wxString::Format("PointBmp%d_%d", i,j));
						m_Properties.erase(m_Properties.find(wxString::Format("pos%d_%d", i,j)));
						m_Properties.erase(m_Properties.find(wxString::Format("PointBmp%d_%d", i,j)));
					}
					bmp_index++;
				}
				m_Properties.erase(m_Properties.find(wxString::Format("point_property_%d", i)));
				m_Properties.erase(m_Properties.find(wxString::Format("point_num_%d", i)));
			}
			bmp_num[i] = save_point_num[i];
			save_point_num[i] = 0;
		}
	}

	/* Re-add attributes and assign values */
	save_index = save_line_chart_num;
	save_line_chart_num = GetIntPropValue(PROP_TOTAL_NUM);
	bmp_index = 0;
	for (int i = 0; i < GetIntPropValue(PROP_TOTAL_NUM); i++)
	{
		U32 point_num = 5;
		U32 line_color = 0;
		U32 point_clolr = 0;
		U32 text_clolr = 0;
		m_Properties.insert(pair<wxString, PropertyBase*>(wxString::Format("line_chart_%d", i), new PropertyFold(wxString::Format("line_chart_%d", i), "LineChart Property")));
		/* add the save value for each line chart*/
		if (i < save_index)
		{
			point_num = save_point[i];
			line_color = save_line_color[i];
			point_clolr = save_point_color[i];
			text_clolr = save_text_color[i];
		}
		m_Properties.insert(pair<wxString, PropertyBase*>(wxString::Format("line_color_%d", i), new PropertyColor(line_color)));
		m_Properties.insert(pair<wxString, PropertyBase*>(wxString::Format("point_color_%d", i), new PropertyColor(point_clolr)));
		m_Properties.insert(pair<wxString, PropertyBase*>(wxString::Format("text_color_%d", i), new PropertyColor(text_clolr)));
		m_Properties.insert(pair<wxString, PropertyBase*>(wxString::Format("point_num_%d", i), new PropertyInteger(point_num)));
		m_Properties.insert(pair<wxString, PropertyBase*>(wxString::Format("point_property_%d", i), new PropertyFold(wxString::Format("point_property_%d", i), wxString::Format("line_chart_%d", i))));
		for (unsigned int j = 0; j < point_num; j++)
		{
			int point_pos = save_point_pos[i][j];
			m_Properties.insert(pair<wxString, PropertyBase*>(wxString::Format("pos%d_%d", i,j), new PropertyInteger(point_pos)));
			if (j < bmp_num[i])
			{
				m_Properties.insert(pair<wxString, PropertyBase*>(wxString::Format("PointBmp%d_%d", i,j), new PropertyImage(save_bmp[bmp_index])));
				bmp_index++;
			}
			else
				m_Properties.insert(pair<wxString, PropertyBase*>(wxString::Format("PointBmp%d_%d", i,j), new PropertyImage()));
		}
		if (bmp_num[i] > point_num)
		{
			bmp_index += bmp_num[i] - point_num;
		}
		/* Save the value of point for erase */
		save_point_num[i] = GetIntPropValue(wxString::Format("point_num_%d", i));
	}
}





UIWindow* uiLineChart::guiWindow(void)
{
	if (GetIntPropValue(PROP_TOTAL_NUM) >= MAX_LINE_CHART_NUM)
	{
		SetIntegerProperty(PROP_TOTAL_NUM,		"10");
		wxLogMessage("The number of line chart can support up to 10");
	}

	UIWindow *window = create_line_chart(GetStrPropValue(PROP_NAME));

	set_window_color(window, GetColorPropValue(PROP_BKG_COLOR));
	set_total_line_chart_num(window, GetIntPropValue(PROP_TOTAL_NUM));
	wxRect rect = GetRectPropValue(PROP_POSITION);
	set_line_chart_rect(window, ui_size(rect.width, rect.height));

	for (int i = 0; i < GetIntPropValue(PROP_TOTAL_NUM); i++)
	{
		if (m_Properties.find(wxString::Format("line_color_%d", i)) != m_Properties.end())
		{
			set_line_chart_line_color(window, i, GetColorPropValue(wxString::Format("line_color_%d", i)));
		}
		if (m_Properties.find(wxString::Format("point_color_%d", i)) != m_Properties.end())
		{
			set_line_chart_point_color(window, i, GetColorPropValue(wxString::Format("point_color_%d", i)));
		}
		if (m_Properties.find(wxString::Format("text_color_%d", i)) != m_Properties.end())
		{
			set_line_chart_text_color(window, i, GetColorPropValue(wxString::Format("text_color_%d", i)));
		}
		if (m_Properties.find(wxString::Format("point_num_%d", i)) != m_Properties.end())
		{
			int point_pos[GetIntPropValue(wxString::Format("point_num_%d", i)) - 1] = {0};
			char *bmp_array[GetIntPropValue(wxString::Format("point_num_%d", i)) - 1] = {0};
			for (int j = 0; j < GetIntPropValue(wxString::Format("point_num_%d", i)); j++)
			{
				if (m_Properties.find(wxString::Format("point_num_%d", i)) != m_Properties.end())
				{
					point_pos[j] = GetIntPropValue(wxString::Format("pos%d_%d", i,j));
				}
				if (m_Properties.find(wxString::Format("PointBmp%d_%d", i,j)) != m_Properties.end())
				{
					bmp_array[j] = (char *)GetImagePropValue(wxString::Format("PointBmp%d_%d", i,j));
				}
			}
			set_line_chart_point_array(window, GetIntPropValue(wxString::Format("point_num_%d", i)), i, point_pos);
			set_line_chart_point_bmp(window, i, bmp_array, GetIntPropValue(wxString::Format("point_num_%d", i)));
		}
	}


	set_window_rect(window, ui_rect(rect.x, rect.y, rect.width, rect.height));

	return window;
}


TiXmlElement* uiLineChart::GetXMLElement(void)
{
	AddUpdateChildProperty();
	TiXmlElement* element = uiWindow::GetXMLElement();

	/* append UILabel property */
	element->SetAttribute("totalNum", m_Properties.find(PROP_TOTAL_NUM)->second->xmlString());

	element->InsertEndChild(TiXmlElement("lineChart"));
	TiXmlElement *linechartInfo = element->FirstChildElement("lineChart");
	for (int i = 0; i < GetIntPropValue(PROP_TOTAL_NUM); i++)
	{
		TiXmlElement elem("Info");

		if (m_Properties.find(wxString::Format("line_color_%d", i)) != m_Properties.end())
		{
			elem.SetAttribute("line_color", m_Properties.find(wxString::Format("line_color_%d", i))->second->xmlString());
			elem.SetAttribute("point_color", m_Properties.find(wxString::Format("point_color_%d", i))->second->xmlString());
			elem.SetAttribute("text_color", m_Properties.find(wxString::Format("text_color_%d", i))->second->xmlString());
			elem.SetAttribute("point_num", m_Properties.find(wxString::Format("point_num_%d", i))->second->xmlString());
			for (int j = 0; j < GetIntPropValue(wxString::Format("point_num_%d", i)); j++)
			{
				elem.SetAttribute(wxString::Format("pos_%d", j), m_Properties.find(wxString::Format("pos%d_%d", i,j))->second->xmlString());
				elem.SetAttribute(wxString::Format("PointBmp_%d", j), m_Properties.find(wxString::Format("PointBmp%d_%d", i,j))->second->xmlString());
			}
		}
		linechartInfo->InsertEndChild(elem);
	}

	return element;
}


void uiLineChart::SetProperties(const TiXmlElement* element)
{
    wxSize regionValue;
	int i = 0;

	/* read base property */
	uiWindow::SetProperties(element);
	AddUpdateChildProperty(element);

	/* read UILabel special property */
	const char *value = element->Attribute("totalNum");
	if (dsC2U(value).empty())
	{
		wxLogMessage("%s have no totalNum property!",element->Attribute("name"));
	}
	else
	{
		SetIntegerProperty(PROP_TOTAL_NUM, value);
	}

	const TiXmlElement *linechartInfo = element->FirstChildElement("lineChart");
	const TiXmlElement *Info = linechartInfo->FirstChildElement("Info");
	while (Info)
	{
		value = Info->Attribute("line_color");
		if (dsC2U(value).empty())
		{
			wxLogMessage("%s have no line_color property!",element->Attribute("name"));
		}
		else
		{
			SetColorProperty(wxString::Format("line_color_%d", i), value);
		}

		value = Info->Attribute("point_color");
		if (dsC2U(value).empty())
		{
			wxLogMessage("%s have no point_color property!",element->Attribute("name"));
		}
		else
		{
			SetColorProperty(wxString::Format("point_color_%d", i), value);
		}

		value = Info->Attribute("text_color");
		if (dsC2U(value).empty())
		{
			wxLogMessage("%s have no text_color property!",element->Attribute("name"));
		}
		else
		{
			SetColorProperty(wxString::Format("text_color_%d", i), value);
		}

		value = Info->Attribute("point_num");
		if (dsC2U(value).empty())
		{
			wxLogMessage("%s have no point_num property!",element->Attribute("name"));
		}
		else
		{
			SetIntegerProperty(wxString::Format("point_num_%d", i), value);
		}

		for (int j = 0; j < GetIntPropValue(wxString::Format("point_num_%d", i)); j++)
		{
			value = Info->Attribute(wxString::Format("pos_%d", j));
			if (dsC2U(value).empty())
			{
				wxLogMessage("%s have no %s property!", element->Attribute("name"), wxString::Format("pos_%d", j));
			}
			else
			{
				SetIntegerProperty(wxString::Format("pos%d_%d", i,j), value);
			}

			value = Info->Attribute(wxString::Format("PointBmp_%d", j));
			if (dsC2U(value).empty())
			{
				wxLogMessage("%s have no %s property!", element->Attribute("name"), wxString::Format("PointBmp_%d", j));
			}
			else
			{
				SetImageProperty(wxString::Format("PointBmp%d_%d", i,j), value);
			}
		}
		i++;
		Info = Info->NextSiblingElement("Info");
	}
}


void uiLineChart::OnBuildDeclarationsCode()
{
	AddDeclaration(wxString::Format(wxT("UILineChart *%s = create_line_chart(\"%s\");"), GetStrPropValue(PROP_VAR_NAME), GetName()));
}

void uiLineChart::OnBuildCreatingCode()
{
	uiWindow::OnBuildCreatingCode();

/*	AddBuildingCode(wxString::Format(wxT("set_total_line_chart_num(%s, \"%d\");\n"), GetName(), GetIntPropValue(PROP_TOTAL_NUM)));

//	set_total_line_chart_num(window, GetIntPropValue(PROP_TOTAL_NUM));
//	wxSize text_size = GetSizePropValue(PROP_TEXT_REGION_SIZE);
//	UISize line_chart_size = {text_size.GetWidth(), text_size.GetHeight()};
//	set_line_chart_rect(window, line_chart_size);

	for (int i = 0; i < GetIntPropValue(PROP_TOTAL_NUM); i++)
	{
		if (m_Properties.find(wxString::Format("line_color_%d", i)) != m_Properties.end())
		{
			AddBuildingCode(wxString::Format(wxT("set_line_chart_line_color(%s, %d, 0x%08X);\n"), GetName(), i, GetColorPropValue(wxString::Format("line_color_%d", i))));
			//set_line_chart_line_color(window, i, GetColorPropValue(wxString::Format("line_color_%d", i)));
		}
		if (m_Properties.find(wxString::Format("point_color_%d", i)) != m_Properties.end())
		{
			AddBuildingCode(wxString::Format(wxT("set_line_chart_point_color(%s, %d, 0x%08X);\n"), GetName(), i, GetColorPropValue(wxString::Format("point_color_%d", i))));
			//set_line_chart_point_color(window, i, GetColorPropValue(wxString::Format("point_color_%d", i)));
		}
		if (m_Properties.find(wxString::Format("text_color_%d", i)) != m_Properties.end())
		{
			AddBuildingCode(wxString::Format(wxT("set_line_chart_text_color(%s, %d, 0x%08X);\n"), GetName(), i, GetColorPropValue(wxString::Format("text_color_%d", i))));
			//set_line_chart_text_color(window, i, GetColorPropValue(wxString::Format("text_color_%d", i)));
		}
		if (m_Properties.find(wxString::Format("point_num_%d", i)) != m_Properties.end())
		{
			wxLogMessage(Name.Remove(0, sizeof("Edit_") - 1));
			int point_pos[GetIntPropValue(wxString::Format("point_num_%d", i)) - 1] = {0};
			char *bmp_array[GetIntPropValue(wxString::Format("point_num_%d", i)) - 1] = {0};
			for (int j = 0; j < GetIntPropValue(wxString::Format("point_num_%d", i)); j++)
			{
				if (m_Properties.find(wxString::Format("point_num_%d", i)) != m_Properties.end())
				{
					point_pos[j] = GetIntPropValue(wxString::Format("pos%d_%d", i,j));
				}
				if (m_Properties.find(wxString::Format("PointBmp%d_%d", i,j)) != m_Properties.end())
				{
					bmp_array[j] = (char *)GetImagePropValue(wxString::Format("PointBmp%d_%d", i,j));
				}
			}
			AddBuildingCode(wxString::Format(wxT("set_line_chart_point_array(%s, %d, %d, %s);\n"), GetName(), GetIntPropValue(wxString::Format("point_num_%d", i)), i, point_pos));
			set_line_chart_point_array(window, GetIntPropValue(wxString::Format("point_num_%d", i)), i, point_pos);
			set_line_chart_point_bmp(window, i, bmp_array, GetIntPropValue(wxString::Format("point_num_%d", i)));
		}
	}*/
	AddBuildingCode("\n");
}

void uiLineChart::OnBuildIdCode()
{

}

