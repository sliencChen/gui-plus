#ifndef X_CONFIGMANAGER_H
#define X_CONFIGMANAGER_H


#include <tinyxml.h>

#include "project.h"

/* ------------------------------------------------------------------------------------------------------------------
*  ConfigManager class
*/
class ConfigManager
{
    friend class CfgMgrBldr;

    TiXmlDocument *doc;
    TiXmlElement* root;
    TiXmlElement* pathNode;

    ConfigManager(TiXmlElement* r);
    TiXmlElement* AssertPath(wxString& path);
    TiXmlElement* GetUniqElement(TiXmlElement* p, const wxString& q);
    void SetNodeText(TiXmlElement *n, const TiXmlText& t);
    wxString InvalidNameMessage(const wxString& what, const wxString& sub, TiXmlElement *localPath) const;
    inline void Collapse(wxString& str) const;
    static void InitPaths();

    static wxString config_folder;

    static wxString alternate_user_data_path;
    static bool has_alternate_user_data_path;

    static bool CreateDirRecursively(const wxString& full_path, int perms = 0755);

protected:
    //Used by CfgMgrBldr internally by ConfigManager
    static wxString GetUserDataFolder();

public:

    static wxString GetFolder();
    static wxString GetConfigFolder(){ return GetFolder(); }


    /* -----------------------------------------------------------------------------------------------------
    *  Path functions for navigation within your configuration namespace
    */
    wxString GetPath() const;
    void SetPath(const wxString& strPath);
    wxArrayString EnumerateSubPaths(const wxString& path);
    wxArrayString EnumerateKeys(const wxString& path);
    void DeleteSubPath(const wxString& strPath);


    /* -----------------------------------------------------------------------------------------------------
    *  Clear all nodes from your namespace or delete the namespace (removing it from the config file).
    *  WARNING: After Delete() returns, the pointer to your instance is invalid. Before you can call ANY member
    *  function of this class, you have to call Manager::Get()->GetConfigManager() for a valid reference again.
    *  Note that Delete() is inherently thread-unsafe. You delete an entire namespace of data as well as the object
    *  responsible of handling that data! Make sure you know what you do.
    *  This is even more true for DeleteAll() which you should really NEVER use.
    */
    void Clear();
    void Delete();
    void DeleteAll();
    void Flush();


    /* -----------------------------------------------------------------------------------------------------
    *  Standard primitives
    */
    void Write(const wxString& name,  const wxString& value, bool ignoreEmpty = false);
    wxString Read(const wxString& key, const wxString& defaultVal = wxEmptyString);
    bool Read(const wxString& key, wxString* str);
    void Write(const wxString& key, const char* str);

    void Write(const wxString& name,  int value);
    bool Read(const wxString& name,  int* value);
    int  ReadInt(const wxString& name,  int defaultVal = 0);

    void Write(const wxString& name,  bool value);
    bool Read(const wxString& name,  bool* value);
    bool ReadBool(const wxString& name,  bool defaultVal = false);

    void Write(const wxString& name,  double value);
    bool Read(const wxString& name,  double* value);
    double ReadDouble(const wxString& name,  double defaultVal = 0.0f);


    /* -----------------------------------------------------------------------------------------------------
    *  Set and unset keys, or test for existence. Note that these functions cannot be used to remove paths
    *  or test existence of paths (it may be used to implicitly create paths, though).
    */
    bool Exists(const wxString& name);
    void Set(const wxString& name);
    void UnSet(const wxString& name);


    /* -----------------------------------------------------------------------------------------------------
    *  Compound objects
    */
    void Write(const wxString& name,  const wxArrayString& as);
    void Read(const wxString& name,  wxArrayString* as);
    wxArrayString ReadArrayString(const wxString& name);

//    void WriteBinary(const wxString& name,  const wxString& source);
//    void WriteBinary(const wxString& name,  void* ptr, size_t len);
//    wxString ReadBinary(const wxString& name);

    void Write(const wxString& name,  const wxColour& c);
    bool Read(const wxString& name, wxColour* value);
    wxColour ReadColour(const wxString& name, const wxColour& defaultVal = *wxBLACK);


    /* view window */
    void WriteWindowViewInfo(const wxString& name);
    void SetDefaultWindowViewValue(void);
    bool ReadWindowView(const wxString& name);
    bool PageListView;
	bool WindowTreeView;
	bool PropGridView;
	bool OutputView;
};


class  CfgMgrBldr
{
    friend class ConfigManager;

    TiXmlDocument *doc;
    TiXmlDocument *volatile_doc;
    wxCriticalSection cs;

    wxString cfg;

    void Flush();
    void Close();
    void SwitchTo(const wxString& absFN);

    ConfigManager* Build(const wxString& name_space);
    wxString FindConfigFile(const wxString& filename);

protected:
    CfgMgrBldr();
    ~CfgMgrBldr();

public:
    static ConfigManager* GetConfigManager(const wxString& name_space);
};



#endif
