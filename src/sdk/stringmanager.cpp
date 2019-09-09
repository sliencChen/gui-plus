
#include "project.h"
#include "main.h"
#include "sdk_common.h"

StringManager *Stringmanager = nullptr;

StringManager* StringManager::Get(void)
{
	if (Stringmanager == nullptr)
	{
		Stringmanager = new StringManager();
	}
	return Stringmanager;
}

StringManager::StringManager(void)
{
	m_Stringpaths = wxEmptyString;
language_save_index = 0;
}

StringManager::~StringManager(void)
{
	if (Stringmanager != NULL)
	{
		delete Stringmanager;
	}
}

void StringManager::DeleteLanguageData(void)
{
	int i, item_num = 0;
	if (StringDataManager != NULL)
	{
		for (i = 0; i < StringDataManager->item_num; i++)
		{
			if (strcmp(StringDataManager->string_items[i].language, "ttx") == 0
				|| strcmp(StringDataManager->string_items[i].language, "id") == 0)	/* it is teletext data, not a language */
			{
				item_num++;
				continue;
			}
			Manager::Get()->GetMainFrame()->LanguageOption->Destroy(IDM_LANGUAGE_BEGIN + get_language_index(i - item_num));
		}
		m_Stringpaths = wxEmptyString;
		language_save_index = 0;
	}
}

void StringManager::LoadLanguageData(void)
{
	int i, item_num = 0;
	load_string_file(m_Stringpaths);
	StringDataManager = GetManager();
	SetIDLanguage();
	if (StringDataManager != NULL)
	{
		for (i = 0; i < StringDataManager->item_num; i++)
		{
			if (strcmp(StringDataManager->string_items[i].language, "ttx") == 0
				|| strcmp(StringDataManager->string_items[i].language, "id") == 0)	/* it is teletext data, not a language */
			{
				item_num++;
				continue;
			}
				Manager::Get()->GetMainFrame()->LanguageOption->AppendRadioItem(IDM_LANGUAGE_BEGIN + get_language_index(i - item_num), dsC2U(get_language_name(get_language_index(i - item_num))));
		}
		SetLanguage(language_save_index);
	}
}

void StringManager::SetLanguage(int language)
{
	language_save_index = language;
	set_language(get_mark_by_language_index(language));
}

wxString StringManager::GetString(int str_id, BOOL InternalString)
{
	if (InternalString)
	{
		return dsC2U(GetInternalString(str_id));
	}
	else
	{
		return dsC2U(GetIDString(str_id));
	}
}

BOOL StringManager::check_is_internal_string(char* StringName)
{
	int index;
	if (StringDataManager == NULL)
	{
		return FALSE;
	}
	if (all_strings != NULL && StringName != NULL)
	{
		for (index = 0; index < str_max_strings; index++)
		{
			if (strcmp(all_strings[index], StringName) == 0)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

//-------------------------------------------------------------------------------------------------
// local function
//-------------------------------------------------------------------------------------------------
const char* StringManager::GetIDString(int str_id)
{
	if (StringDataManager == NULL)
	{
		return "No init string manager";
	}
	if (id_strings != NULL)
	{
		return id_strings[str_id];
	}
	return "No Such String";
}

const char* StringManager::GetInternalString(int str_id)
{
	if (StringDataManager == NULL)
	{
		return "No init string manager";
	}
	if (str_id < str_max_strings && all_strings != NULL)
	{
		return all_strings[str_id];
	}
	return "No Such String";
}

void StringManager::SetIDLanguage(void)
{
	int index;
	for (index = 0; index < StringDataManager->item_num; index++) /* id language must be before en language */
	{
		if (strcmp(StringDataManager->string_items[index].language, "id") == 0)
		{
			set_id_language(index);
			break;
		}
		else if (strcmp(StringDataManager->string_items[index].language, "en") == 0)
		{
			set_id_language(index);
			break;
		}
	}
	id_strings = all_strings;
}

//-------------------------------------------------------------------------------------------------
// function about parameter save language
//-------------------------------------------------------------------------------------------------
int StringManager::GetSaveLanguageIndex(void)
{
	return language_save_index;
}

const char* StringManager::GetSaveLanguageMark(void)
{
	return get_mark_by_language_index(language_save_index);
}

void StringManager::SetSaveLanguageIndex(char *language_mark)
{
	language_save_index = get_language_index_by_string_mark(language_mark);
}

char* StringManager::SetIDStringByRealString(char* language)
{
	int index;
	if (StringDataManager == NULL)
	{
		return language;
	}
	if (all_strings != NULL && id_strings != NULL && language != NULL)
	{
		for (index = 0; index < str_max_strings; index++)
		{
			if (strcmp(all_strings[index], language) == 0)
			{
				return id_strings[index];
			}
		}
	}
	return language;
}

char* StringManager::SetRealStringByIDString(char* id_string)
{
	int index;
	if (StringDataManager == NULL)
	{
		return id_string;
	}
	if (all_strings != NULL && id_strings != NULL && id_string != NULL)
	{
		for (index = 0; index < str_max_strings; index++)
		{
			if (strcmp(id_strings[index], id_string) == 0)
			{
				return all_strings[index];
			}
		}
	}
	return id_string;
}


