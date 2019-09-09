/*
 * uiline_chart.h
 *
 *  Created on: 2019-8-2
 *      Author: Antony
 */

#ifndef _UILINE_CHART_H_
#define _UILINE_CHART_H_


#include "uiwindow.h"


class uiLineChart : public uiWindow
{
public:
	uiLineChart(uiWindow *parent = nullptr);
	~uiLineChart(void);

	wxPGChoices AddEnmuChildProperty(wxString key);
	void AddUpdateChildProperty(const TiXmlElement* element = nullptr);

	UIWindow* guiWindow(void);

	TiXmlElement* GetXMLElement(void);

	void SetProperties(const TiXmlElement* element);


private:
	virtual void OnBuildDeclarationsCode();
	virtual void OnBuildCreatingCode();
	virtual void OnBuildIdCode();
	int save_line_chart_num = 0;
	int save_point_num[MAX_LINE_CHART_NUM] = {0};
	bool first_open_window = true;
};


#endif /* _UILINE_CHART_H_ */

