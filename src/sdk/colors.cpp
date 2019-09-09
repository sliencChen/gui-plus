/*
 * colors.cpp
 *
 *  Created on: 2019年4月22日
 *      Author: Brant
 */

#include <string.h>
#include "sdk_common.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include "colors.h"


#define PROJECT_COLOR_START_INDEX		100


static Colors *s_SharedColorsInstance = nullptr;


Colors* Colors::Get(void)
{
	if (s_SharedColorsInstance == nullptr)
	{
		s_SharedColorsInstance = new Colors();
	}
	return s_SharedColorsInstance;
}

unsigned int Colors::CustomColorId(void)
{
	return wxPG_COLOUR_CUSTOM;
}



Colors::Colors(void)
{
	m_NormColors.Empty();

	m_NormColors.AddColor(uiColor(0x00000000, "Custom"));
	m_NormColors.AddColor(uiColor(0x00000000, "Transparent"));
	m_NormColors.AddColor(uiColor(0x00FF00FF, "Clear"));
	m_NormColors.AddColor(uiColor(0xFFFF0000, "Red"));
	m_NormColors.AddColor(uiColor(0xFFFFFF00, "Yellow"));
	m_NormColors.AddColor(uiColor(0xFF0000FF, "Blue"));
	m_NormColors.AddColor(uiColor(0xFF00FF00, "Green"));
	m_NormColors.AddColor(uiColor(0xFF000000, "Black"));
	m_NormColors.AddColor(uiColor(0xFFFFFFFF, "White"));
	m_NormColors.AddColor(uiColor(0xFFC0C0C0, "Silver"));
	m_NormColors.AddColor(uiColor(0xFF808080, "Gray"));
	m_NormColors.AddColor(uiColor(0xFFFFA500, "Orange"));
	m_NormColors.AddColor(uiColor(0xFFFFD700, "Gold"));
	m_NormColors.AddColor(uiColor(0xFF006400, "Dark Green"));
	m_NormColors.AddColor(uiColor(0xFF808000, "Olive"));
	m_NormColors.AddColor(uiColor(0xFF800080, "Purple"));
	m_NormColors.AddColor(uiColor(0xFF008080, "Teal"));
	m_NormColors.AddColor(uiColor(0xFF000080, "Navy"));
	m_NormColors.AddColor(uiColor(0xFF00FFFF, "Cyan"));
	m_NormColors.AddColor(uiColor(0xFFFF00FF, "Magenta"));
}

Colors::~Colors(void)
{
	ClearNames();
	delete m_ColorIndexs;
}

void Colors::Refresh(Project* project)
{
	if (project)
		m_ProjColors = project->GetColorList();
	else if (Manager::Get()->GetActiveProject())
		m_ProjColors = Manager::Get()->GetActiveProject()->GetColorList();
	else
		m_ProjColors.Empty();


	/* delete old datas */
	ClearNames();
	delete m_ColorIndexs;
	m_ColorIndexs = nullptr;

	unsigned int totalColor = m_NormColors.Count() + m_ProjColors.Count();
	m_ColorNum = totalColor;
	m_ColorNames = new char*[totalColor + 1];	/* one more for NULL, this array must end with NULL */
	m_ColorIndexs = new long[totalColor];
	memset(m_ColorNames, 0, (totalColor + 1) * sizeof(char*));
	memset(m_ColorIndexs, 0, totalColor * sizeof(long));

	unsigned int i;
	const char *colorName;
	for (i = 0; i < totalColor; i++)
	{
		if (i < m_NormColors.Count())
		{
			m_ColorIndexs[i] = i > 0 ? i : wxPG_COLOUR_CUSTOM;
			colorName = dsU2C(m_NormColors.GetColor(i).GetName());
		}
		else
		{
			m_ColorIndexs[i] = PROJECT_COLOR_START_INDEX + i - m_NormColors.Count();
			colorName = dsU2C(m_ProjColors.GetColor(i - m_NormColors.Count()).GetName());
		}

	    char *name = new char[strlen(colorName) + 1];
	    strcpy(name, colorName);
	    m_ColorNames[i] = name;
	}
}

const char* const *Colors::GetNames(void)
{
	if (m_ColorNames == nullptr)
		Refresh();

	return m_ColorNames;
}

const long* Colors::GetIds(void)
{
	if (m_ColorIndexs == nullptr)
		Refresh();

	return m_ColorIndexs;
}

const long Colors::GetColorId(const wxString& name)
{
	unsigned int customColorId = Colors::CustomColorId();

	if (name.IsEmpty())
		return customColorId;

	unsigned int i;
	for (i = 0; i < m_NormColors.Count(); i++)
	{
		/* skip custom */
		if (i != customColorId && m_NormColors.GetColor(i).GetName() == name)
			return i;
	}
	for (i = PROJECT_COLOR_START_INDEX; i < PROJECT_COLOR_START_INDEX + m_ProjColors.Count(); i++)
	{
		if (i != customColorId && m_ProjColors.GetColor(i - PROJECT_COLOR_START_INDEX).GetName() == name)
			return i;
	}

	return customColorId;
}

bool Colors::IsCorrectColorName(const wxString& name)
{
	return (unsigned int)GetColorId(name) != Colors::CustomColorId();
}


wxColour Colors::GetColor(unsigned int id)
{
	unsigned int color;

	if (id < PROJECT_COLOR_START_INDEX)
		color = m_NormColors.GetColor(id).m_Value;
	else
		color = m_ProjColors.GetColor(id - PROJECT_COLOR_START_INDEX).m_Value;

	unsigned char a, r, g, b;
	a = (unsigned char)((color & 0xFF000000) >> 24);
    r = (unsigned char)((color & 0x00FF0000) >> 16);
    g = (unsigned char)((color & 0x0000FF00) >> 8);
    b = (unsigned char)((color & 0x000000FF));

    return wxColour(r, g, b, a);
}

wxString Colors::GetName(int id)
{
	if (id < PROJECT_COLOR_START_INDEX)
		return m_NormColors.GetColor(id).GetName();

	return m_ProjColors.GetColor(id - PROJECT_COLOR_START_INDEX).GetName();
}


void Colors::ClearNames(void)
{
	for (unsigned int i = 0; i < m_ColorNum; i++)
	{
		delete m_ColorNames[i];
	}
	delete m_ColorNames;

	m_ColorNames = nullptr;
}
