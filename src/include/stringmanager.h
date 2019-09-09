#ifndef X_STRINGMANAGER_H
#define X_STRINGMANAGER_H

#include "../sdk/stbgui/include/gui_common.h"
/* ------------------------------------------------------------------
*  Class StringManager
*/

class StringManager
{
public:
	static StringManager* Get(void);

	StringManager(void);
	~StringManager(void);
	void DeleteLanguageData(void);
	void LoadLanguageData(void);
	wxString GetString(int str_id, BOOL InternalString);
	void SetLanguage(int language);
	int GetSaveLanguageIndex(void);
	void SetSaveLanguageIndex(char *language_mark);
	const char* GetSaveLanguageMark(void);
	char* SetIDStringByRealString(char* language);
	char* SetRealStringByIDString(char* id_string);

	BOOL check_is_internal_string(char* StringName);
	wxString m_Stringpaths;
private:
	void SetIDLanguage(void);
	const char* GetIDString(int str_id);
	const char* GetInternalString(int str_id);
	char **id_strings;
	int language_save_index;
	StringListManager           *StringDataManager      = nullptr;
};

#endif
