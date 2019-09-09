/*
 * colors.h
 *
 *  Created on: 2019-4-22
 *      Author: Brant
 */

#ifndef _COLORS_H_
#define _COLORS_H_


class Colors
{
public:
	static Colors* Get(void);
	static unsigned int CustomColorId(void);

public:
	Colors(void);
	~Colors(void);

	/* need call it when project color changed */
	void Refresh(Project *project = nullptr);

	/* for property grid editor */
	const char* const *GetNames(void);
	const long* GetIds(void);

	const long GetColorId(const wxString& name);
	wxColour GetColor(unsigned int id);
	wxString GetName(int id);

	bool IsCorrectColorName(const wxString& name);

private:
	ColorList m_NormColors;
	ColorList m_ProjColors;

	char **m_ColorNames = nullptr;
	long* m_ColorIndexs = nullptr;
	unsigned int m_ColorNum = 0;

	void ClearNames(void);
};


#endif /* _COLORS_H_ */
