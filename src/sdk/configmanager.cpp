/*
 * configmanager.cpp
 *
 *  Created on: 2019-7-5
 *      Author: Brant
 */


#include <wx/wx.h>
#include <wx/filename.h>
//
#include "sdk_common.h"
#include "configmanager.h"
#include "sdk_common.h"

//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------
wxString ConfigManager::config_folder;
wxString ConfigManager::alternate_user_data_path;
bool ConfigManager::has_alternate_user_data_path=false;

static CfgMgrBldr* s_SharedCfgMgrBldrInstance = nullptr;


namespace CfgMgrConsts
{
    const wxString app_path(_T("app_path"));
    const wxString data_path(_T("data_path"));
    const wxString dotDot(_T(".."));
    const int version = 1;
}

//-------------------------------------------------------------------------------------------------
// ConfigManager
//-------------------------------------------------------------------------------------------------
ConfigManager::ConfigManager(TiXmlElement* r) : doc(r->GetDocument()), root(r), pathNode(r)
{
	PageListView = true;
	WindowTreeView = true;
	PropGridView = true;
	OutputView = true;
}


//-------------------------------------------------------------------------------------------------
// private - internal
//-------------------------------------------------------------------------------------------------
inline void ConfigManager::Collapse(wxString& str) const
{
    const wxChar *src = str.c_str();
    wxChar *dst = (wxChar*) src;
    wxChar c;
    size_t len = 0;

    while ((c = *src))
    {
        ++src;

        *dst = c;
        ++dst;
        ++len;

        if (c == _T('/'))
        while (*src == _T('/'))
            ++src;
    }
    str.Truncate(len);
}

bool ConfigManager::CreateDirRecursively(const wxString& full_path, int perms)
{
    wxFileName tmp(full_path);
    if (wxDirExists(tmp.GetPath())) // early out, even if full_path is a filename, but the path already exists
        return true;

    wxArrayString dirs;
    wxString currdir;
    currdir = tmp.GetVolume() + tmp.GetVolumeSeparator() + wxFILE_SEP_PATH;
    dirs = tmp.GetDirs();

    for (size_t i = 0; i < dirs.GetCount(); ++i)
    {
        currdir << dirs[i];
        if (!wxDirExists(currdir) && !wxMkdir(currdir, perms))
            return false;
        currdir << wxFILE_SEP_PATH;
    }
    return true;
}


void ConfigManager::InitPaths()
{
    ConfigManager::config_folder = ConfigManager::GetUserDataFolder();

    ConfigManager::CreateDirRecursively(ConfigManager::config_folder);
}

wxString ConfigManager::GetFolder()
{
    static bool once = 1;

    if (once)
    {
        InitPaths();
        once = false;
    }

    return ConfigManager::config_folder;
}


//-------------------------------------------------------------------------------------------------
// protected - internal
//-------------------------------------------------------------------------------------------------
inline wxString ConfigManager::GetUserDataFolder()
{
    if (has_alternate_user_data_path)
        return alternate_user_data_path;

    TCHAR buffer[MAX_PATH];
    if (!ConfigManager::has_alternate_user_data_path && ::GetEnvironmentVariable(_T("APPDATA"), buffer, MAX_PATH))
        return wxString::Format(_T("%s\\Designer\\"), buffer);
    else
        return wxString::Format(_T("C:\\Users\\Administrator\\AppData\\Roaming\\Designer\\"));
}


/*------------------------------------------------------------------------------------------------------------------
*  Hack to enable Turkish language. wxString::Upper will convert lowercase 'i' to \u0130 instead of \u0069 in Turkish locale,
*  which will break the config file when used in a tag
*/
inline void to_upper(wxString& s)
{
    wxChar *p = (wxChar*) s.wc_str();
    wxChar q;

    size_t len = s.length()+1;
    for (;--len;++p)
    {
        q = *p;
        if (q >= 'a' && q <= 'z')
            *p = q - 32;
    }
}

inline void to_lower(wxString& s)
{
    wxChar *p = (wxChar*) s.wc_str();
    wxChar q;

    size_t len = s.length()+1;
    for (;--len;++p)
    {
        q = *p;
        if (q >= 'A' && q <= 'Z')
            *p = q + 32;
    }
}


/* ------------------------------------------------------------------------------------------------------------------
*  Configuration path handling
*  Note that due to namespaces, you do no longer need to care about saving/restoring the current path in the normal case.
*  Mostly, there will be only one module working with one namespace, and every namespace keeps track of its own path.
*  Also, please note that GetPath() is more expensive than it seems (not a mere accessor to a member variable!), while
*  SetPath() not only sets the current path, but also creates the respective nodes in the XML document if these don't exist.
*/

wxString ConfigManager::GetPath() const
{
    TiXmlElement *e = pathNode;
    wxString ret;
    ret.Alloc(64);

    ret = dsC2U(e->Value());
    while ((e = e->Parent()->ToElement()) && e != root)
    {
        ret.Prepend(_T('/'));
        ret.Prepend(dsC2U(e->Value()));
    }
    ret.Prepend(_T('/'));
    return ret;
}

void ConfigManager::SetPath(const wxString& path)
{
    wxString p(path + _T('/'));
    pathNode = AssertPath(p);
}

wxString ConfigManager::InvalidNameMessage(const wxString& what, const wxString& sub, TiXmlElement *localPath) const
{
    wxString s;
    s.Printf(_T("The %s %s (child of node \"%s\" in namespace \"%s\") does not meet the standard for path naming (must start with a letter)."),
    what.c_str(),
    sub.c_str(),
    dsC2U(localPath->Value()).c_str(),
    dsC2U(root->Value()).c_str());

    return s;
}


TiXmlElement* ConfigManager::AssertPath(wxString& path)
{
    Collapse(path);

    wxString illegal(_T(" -:.\"\'$&()[]<>+#"));
    size_t i = 0;
    while ((i = path.find_first_of(illegal, i)) != wxString::npos)
        path[i] = _T('_');

    TiXmlElement *localPath = pathNode ? pathNode : root;

    if (path.GetChar(0) == '/')  // absolute path
    {
        localPath = root;
        path = path.Mid(1);
    }

    if (path.find(_T('/')) != wxString::npos) // need for path walking
    {
        to_lower(path);
    }


    wxString sub;

    while (path.find(_T('/')) != wxString::npos)
    {
        sub = path.BeforeFirst(_T('/'));
        path = path.AfterFirst(_T('/'));

        if (localPath != root && sub.IsSameAs(CfgMgrConsts::dotDot))
            localPath = localPath->Parent()->ToElement();
        else if (sub.GetChar(0) < _T('a') || sub.GetChar(0) > _T('z'))
        {
//            cbThrow(InvalidNameMessage(_T("subpath"), sub, localPath));
        }
        else
        {
            TiXmlElement* n = localPath->FirstChildElement(dsC2U(sub));
            if (n)
                localPath = n;
            else
                localPath = (TiXmlElement*) localPath->InsertEndChild(TiXmlElement(dsC2U(sub)));
        }
    }

    to_upper(path);

    if (!path.IsEmpty() && (path.GetChar(0) < _T('A') || path.GetChar(0) > _T('Z')))
    {
//        cbThrow(InvalidNameMessage(_T("key"), path, localPath));
    }


    return localPath;
}


wxArrayString ConfigManager::EnumerateSubPaths(const wxString& path)
{
    wxString key(path + _T('/')); // the trailing slash hack is required because AssertPath expects a key name
    TiXmlNode* e = AssertPath(key);
    wxArrayString ret;

    TiXmlElement *curr = nullptr;
    if (e)
    {
        while (e->IterateChildren(curr) && (curr = e->IterateChildren(curr)->ToElement()))
        {

//            wxChar c = *(dsC2U(curr->Value()));
//            if (c < _T('A') || c > _T('Z')) // first char must be a letter, uppercase letters are key names
//                ret.Add(dsC2U(curr->Value()));
            wxString value = dsC2U(curr->Value());
            wxUniChar firstChar = value.at(0);

            if (firstChar < _T('A') || firstChar > _T('Z')) // opposite of the above
                ret.Add(value);
        }
    }
    return ret;
}

void ConfigManager::DeleteSubPath(const wxString& thePath)
{
    if (doc->ErrorId())
    {
//        cbMessageBox(wxString(_T("### TinyXML error:\n")) << dsC2U(doc->ErrorDesc()));
        doc->ClearError();
    }

    wxString path(thePath);
    to_lower(path);

    Collapse(path);

    wxString illegal(_T(" :.,;!\"\'$%&()[]<>{}?*+-|#"));
    size_t i;
    while ((i = path.find_first_of(illegal)) != wxString::npos)
        path[i] = _T('_');

    if (path.Last() == _T('/'))
        path.RemoveLast();

    if (path.IsSameAs(_T("/"))) // this function will refuse to remove root!
        return;

    TiXmlElement* parent = pathNode ? pathNode : root;

    if (path.find(_T('/')) != wxString::npos)
    {
        wxString sub;
        do
        {
            sub = path.BeforeFirst(_T('/'));
            path = path.AfterFirst(_T('/'));

            if (sub.IsEmpty())
                parent = root;
            else if (sub.IsSameAs(_T(".")))
                ;
            else if (parent != root && sub.IsSameAs(_T("..")))
                parent = parent->Parent()->ToElement();
            else
            {
                TiXmlElement* n = parent->FirstChildElement(dsU2C(sub));
                if (n)
                    parent = n;
                else
                    return;
            }
        }
        while (path.find(_T('/')) != wxString::npos);
    }

    if (!path.IsEmpty())
    {
        if (TiXmlNode *toRemove = parent->FirstChild(dsU2C(path)))
        {
            toRemove->Clear();
            parent->RemoveChild(toRemove);
        }
    }
}


wxArrayString ConfigManager::EnumerateKeys(const wxString& path)
{
    wxString key(path + _T('/')); // the trailing slash hack is required because AssertPath expects a key name
    TiXmlNode* e = AssertPath(key);
    wxArrayString ret;

    TiXmlElement *curr = nullptr;
    if (e)
    {
        while (e->IterateChildren(curr) && (curr = e->IterateChildren(curr)->ToElement()))
        {
//            wxChar c = *(dsC2U(curr->Value()));
            wxString value = dsC2U(curr->Value());
            wxUniChar firstChar = value.at(0);

//            if (c >= _T('A') && c <= _T('Z')) // opposite of the above
//                ret.Add(dsC2U(curr->Value()));
            if (firstChar >= _T('A') && firstChar <= _T('Z')) // opposite of the above
                ret.Add(value);
        }
    }
    return ret;
}


/* -----------------------------------------------------------------------------------------------------
*  Clear all nodes from your namespace or delete the namespace alltogether (removing it from the config file).
*  After Delete() returns, the pointer to your instance is invalid.
*/

void ConfigManager::Clear()
{
    root->Clear();
}

void ConfigManager::Delete()
{
    CfgMgrBldr * bld = s_SharedCfgMgrBldrInstance;
    const wxString ns(dsC2U(root->Value()));

    root->Clear();
    doc->RootElement()->RemoveChild(root);

    wxCriticalSectionLocker(bld->cs);

    delete this;
}

void ConfigManager::DeleteAll()
{
    CfgMgrBldr * bld = s_SharedCfgMgrBldrInstance;
    const wxString ns(dsC2U(root->Value()));

//    if (!ns.IsSameAs(_T("app")))
//        cbThrow(_T("Illegal attempt to invoke DeleteAll()."));

    wxCriticalSectionLocker(bld->cs);
    doc->RootElement()->Clear();

}

void ConfigManager::Flush()
{
    CfgMgrBldr * bld = s_SharedCfgMgrBldrInstance;
    wxCriticalSectionLocker(bld->cs);
    bld->Flush();
}


/* ------------------------------------------------------------------------------------------------------------------
*  Utility functions for writing nodes
*/

TiXmlElement* ConfigManager::GetUniqElement(TiXmlElement* p, const wxString& q)
{
    TiXmlElement* r;
    if ((r = p->FirstChildElement(dsU2C(q))))
        return r;

    return (TiXmlElement*)(p->InsertEndChild(TiXmlElement(dsU2C(q))));
}

void ConfigManager::SetNodeText(TiXmlElement* n, const TiXmlText& t)
{
    TiXmlNode *c = n->FirstChild();
    if (c)
        n->ReplaceChild(c, t);
    else
        n->InsertEndChild(t);
}


/* ------------------------------------------------------------------------------------------------------------------
*  Write and read values
*  Regardless of namespaces, the string keys app_path and data_path always refer to the location of the application's executable
*  and the data path, respectively. These values are never saved to the configuration, but kept in static variables.
*  The application makes use of this by "writing" to the configuration file after determining these values at runtime.
*/
void ConfigManager::Write(const wxString& name,  const wxString& value, bool ignoreEmpty)
{
//    if (name.IsSameAs(CfgMgrConsts::app_path))
//    {
//        return;
//    }
//    else if (name.IsSameAs(CfgMgrConsts::data_path))
//    {
//        data_path_global = value;
//        return;
//    }
    if (ignoreEmpty && value.IsEmpty())
    {
        UnSet(name);
        return;
    }

    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlElement *str = GetUniqElement(e, key);

    TiXmlElement *s = GetUniqElement(str, _T("str"));

    TiXmlText t(value.mb_str(wxConvUTF8));
    t.SetCDATA(true);
    SetNodeText(s, t);
}

void ConfigManager::Write(const wxString& key, const char* str)
{
    /* NOTE (mandrav#1#): Do *not* remove 'false' from the call because in ANSI builds,
    it matches this very function and overflows the stack... */
    Write(key, dsC2U(str), false);
}

wxString ConfigManager::Read(const wxString& name, const wxString& defaultVal)
{
//    if (name.IsSameAs(CfgMgrConsts::app_path))
//        return app_path;
//    else if (name.IsSameAs(CfgMgrConsts::data_path))
//        return data_path_global;

    wxString ret;

    if (Read(name, &ret))
        return ret;
    else
        return defaultVal;
}

bool ConfigManager::Read(const wxString& name, wxString* str)
{
//    if (name.IsSameAs(CfgMgrConsts::app_path))
//    {
//        str->assign(app_path);
//        return true;
//    }
//    else if (name.IsSameAs(CfgMgrConsts::data_path))
//    {
//        str->assign(data_path_global);
//        return true;
//    }

    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlHandle parentHandle(e);
    TiXmlText *t = (TiXmlText *) parentHandle.FirstChild(dsU2C(key)).FirstChild("str").FirstChild().Node();

    if (t)
    {
        str->assign(dsC2U(t->Value()));
        return true;
    }
    return false;
}


void ConfigManager::Write(const wxString& name,  int value)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);
    TiXmlElement *leaf = GetUniqElement(e, key);

    leaf->SetAttribute("int", value);
}

int  ConfigManager::ReadInt(const wxString& name,  int defaultVal)
{
    int ret;

    if (Read(name, &ret))
        return ret;
    else
        return defaultVal;
}

bool ConfigManager::Read(const wxString& name,  int* value)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlHandle parentHandle(e);
    TiXmlElement *leaf = parentHandle.FirstChild(dsU2C(key)).Element();

    if (leaf)
        return leaf->QueryIntAttribute("int", value) == TIXML_SUCCESS;
    return false;
}


void ConfigManager::Write(const wxString& name,  bool value)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);
    TiXmlElement *leaf = GetUniqElement(e, key);

    leaf->SetAttribute("bool", value ? "1" : "0");
}

bool  ConfigManager::ReadBool(const wxString& name,  bool defaultVal)
{
    bool ret;

    if (Read(name, &ret))
        return ret;
    else
        return defaultVal;
}

bool ConfigManager::Read(const wxString& name,  bool* value)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlHandle parentHandle(e);
    TiXmlElement *leaf = parentHandle.FirstChild(dsU2C(key)).Element();

    if (leaf && leaf->Attribute("bool"))
    {
        *value = leaf->Attribute("bool")[0] == '1';
        return true;
    }
    return false;
}


void ConfigManager::Write(const wxString& name,  double value)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);
    TiXmlElement *leaf = GetUniqElement(e, key);

    leaf->SetDoubleAttribute("double", value);
}

double  ConfigManager::ReadDouble(const wxString& name,  double defaultVal)
{
    double ret;

    if (Read(name, &ret))
        return ret;
    else
        return defaultVal;
}

bool ConfigManager::Read(const wxString& name,  double* value)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlHandle parentHandle(e);
    TiXmlElement *leaf = parentHandle.FirstChild(dsU2C(key)).Element();

    if (leaf)
        return leaf->QueryDoubleAttribute("double", value) == TIXML_SUCCESS;
    return false;
}


void ConfigManager::Set(const wxString& name)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);
    GetUniqElement(e, key);
}

void ConfigManager::UnSet(const wxString& name)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlNode *leaf = GetUniqElement(e, key);
    e->RemoveChild(leaf);
}

bool ConfigManager::Exists(const wxString& name)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlHandle parentHandle(e);
    TiXmlElement *leaf = parentHandle.FirstChild(dsU2C(key)).Element();

    return leaf;
}


void ConfigManager::Write(const wxString& name,  const wxArrayString& arrayString)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlElement *leaf = GetUniqElement(e, key);

    TiXmlElement *as;
    as = GetUniqElement(leaf, _T("astr"));
    leaf->RemoveChild(as);
    as = GetUniqElement(leaf, _T("astr"));

    for (unsigned int i = 0; i < arrayString.GetCount(); ++i)
    {
        TiXmlElement s("s");

        TiXmlText t(arrayString[i].mb_str(wxConvUTF8));
        t.SetCDATA(true);

        s.InsertEndChild(t);
        as->InsertEndChild(s);
    }
}

void ConfigManager::SetDefaultWindowViewValue(void)
{
}

void ConfigManager::WriteWindowViewInfo(const wxString& name)
{
	wxString key(name);
	TiXmlElement* e = AssertPath(key);

	TiXmlElement *leaf = GetUniqElement(e, key);

	TiXmlElement *as;
	as = GetUniqElement(leaf, _T("ViewVisible"));
	leaf->RemoveChild(as);
	as = GetUniqElement(leaf, _T("ViewVisible"));
	
	as->SetAttribute("PageListView", PageListView);
	as->SetAttribute("WindowTreeView", WindowTreeView);
	as->SetAttribute("PropGridView", PropGridView);
	as->SetAttribute("OutputView", OutputView);

}


void ConfigManager::Read(const wxString& name, wxArrayString *arrayString)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlHandle parentHandle(e);
    TiXmlNode *asNode = parentHandle.FirstChild(dsU2C(key)).FirstChild("astr").Node();

    TiXmlNode *curr = nullptr;
    if (asNode)
    {
        while ((curr = asNode->IterateChildren("s", curr)))
            arrayString->Add(dsC2U(curr->FirstChild()->ToText()->Value()));
    }
}

wxArrayString ConfigManager::ReadArrayString(const wxString& name)
{
    wxArrayString as;
    Read(name, &as);
    return as;
}

bool ConfigManager::ReadWindowView(const wxString& name)
{
	wxString key(name);
	TiXmlElement* e = AssertPath(key);
	TiXmlHandle parentHandle(e);
	TiXmlElement *leaf = parentHandle.FirstChild(dsU2C(key)).Element();
	PageListView = true;
	WindowTreeView = true;
	PropGridView = true;
	OutputView = true;
	if (!leaf || !leaf->FirstChildElement("ViewVisible"))
	{
		return false;
	}
	TiXmlElement* ViewNode = leaf->FirstChildElement("ViewVisible");

	if (ViewNode)
	{
		if (ViewNode->Attribute("PageListView") && ViewNode->Attribute("WindowTreeView") && ViewNode->Attribute("PropGridView") && ViewNode->Attribute("OutputView"))
		{
			ViewNode->QueryBoolAttribute("PageListView", &PageListView);
			ViewNode->QueryBoolAttribute("WindowTreeView", &WindowTreeView);
			ViewNode->QueryBoolAttribute("PropGridView", &PropGridView);
			ViewNode->QueryBoolAttribute("OutputView", &OutputView);
		}
	}

	return true;
}

//void ConfigManager::WriteBinary(const wxString& name,  const wxString& source)
//{
//    wxString key(name);
//    TiXmlElement* e = AssertPath(key);
//
//    TiXmlElement *str = GetUniqElement(e, key);
//
//    TiXmlElement *s = GetUniqElement(str, _T("bin"));
//    s->SetAttribute("crc", wxCrc32::FromString(source));
//    SetNodeText(s, TiXmlText(wxBase64::Encode(source).mb_str(wxConvUTF8)));
//}
//
//void ConfigManager::WriteBinary(const wxString& name,  void* ptr, size_t len)
//{
//    wxString s((wxChar*)ptr, len);
//    WriteBinary(name,  s);
//}
//
//wxString ConfigManager::ReadBinary(const wxString& name)
//{
//    wxString str;
//    wxString key(name);
//    TiXmlElement* e = AssertPath(key);
//    unsigned int crc = 0;
//
//    TiXmlHandle parentHandle(e);
//    TiXmlElement* bin = parentHandle.FirstChild(dsU2C(key)).FirstChild("bin").Element();
//
//    if (!bin)
//        return wxEmptyString;
//
//    if (bin->QueryIntAttribute("crc", (int*)&crc) != TIXML_SUCCESS)
//        return wxEmptyString;
//
//    if (const TiXmlText* t = bin->FirstChild()->ToText())
//    {
//        str.assign(dsC2U(t->Value()));
//        str = wxBase64::Decode(str);
//        if (crc ==  wxCrc32::FromString(str))
//            return str;
//    }
//    return wxEmptyString;
//}


void ConfigManager::Write(const wxString& name,  const wxColour& c)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlElement *leaf = GetUniqElement(e, key);

    TiXmlElement *s = GetUniqElement(leaf, _T("colour"));
    if (c == wxNullColour)
    {
        s->SetAttribute("null", "true");
        s->SetAttribute("r", 0);
        s->SetAttribute("g", 0);
        s->SetAttribute("b", 0);
    }
    else
    {
        s->SetAttribute("r", c.Red());
        s->SetAttribute("g", c.Green());
        s->SetAttribute("b", c.Blue());
    }
}

wxColour ConfigManager::ReadColour(const wxString& name, const wxColour& defaultVal)
{
    wxColour ret;

    if (Read(name, &ret))
        return ret;
    else
        return defaultVal;
}

bool ConfigManager::Read(const wxString& name, wxColour* ret)
{
    wxString key(name);
    TiXmlElement* e = AssertPath(key);

    TiXmlHandle parentHandle(e);
    TiXmlElement *c = (TiXmlElement *) parentHandle.FirstChild(dsU2C(key)).FirstChild("colour").Element();

    if (c)
    {
        const char *isNull = c->Attribute("null");
        if (isNull && strcmp(isNull, "true") == 0)
        {
            *ret = wxNullColour;
            return true;
        }
        else
        {
            int r, g, b;
            if (c->QueryIntAttribute("r", &r) == TIXML_SUCCESS
                    && c->QueryIntAttribute("g", &g) == TIXML_SUCCESS
                    && c->QueryIntAttribute("b", &b) == TIXML_SUCCESS)
            {
                ret->Set(r, g, b);
                return true;
            }
        }
    }
    *ret = wxNullColour;
    return false;
}


/* ------------------------------------------------------------------------------------------------------------------
*  "Builder pattern" class for ConfigManager
*  Do not use this class  -  Manager::Get()->GetConfigManager() is a lot friendlier
*/

CfgMgrBldr::CfgMgrBldr() : doc(nullptr), volatile_doc(nullptr)
{
    TiXmlBase::SetCondenseWhiteSpace(false);
    wxString personality("default");

    cfg = FindConfigFile(personality + _T(".conf"));
//    wxLogMessage("cfg=%s", cfg);

    if (cfg.IsEmpty())
    {
        cfg = ConfigManager::GetConfigFolder() + personality + _T(".conf");
        doc = new TiXmlDocument();
        doc->InsertEndChild(TiXmlDeclaration("1.0", "UTF-8", "yes"));
        doc->InsertEndChild(TiXmlElement("DesignerConfig"));
        doc->FirstChildElement("DesignerConfig")->SetAttribute("version", 1);
//        wxLogMessage("cfg=%s", cfg);
        return;
    }
    SwitchTo(cfg);
}


wxString CfgMgrBldr::FindConfigFile(const wxString& filename)
{
    wxString u(ConfigManager::GetUserDataFolder() + filename);

    if (::wxFileExists(u))
    {
        return u;
    }
    return wxEmptyString;
}


void CfgMgrBldr::SwitchTo(const wxString& fileName)
{
    doc = new TiXmlDocument();

    if (!TinyXML::LoadDocument(fileName, doc))
    {
        const wxString message = wxString::Format(_("Error reading config file: %s"),
                                                  fileName.wx_str());
        wxLogMessage("message = %s", message);
    }

    TiXmlElement* docroot = doc->FirstChildElement("DesignerConfig");
    if (!docroot)
    {
        const wxString message = wxString::Format(wxT("Cannot find docroot in config file '%s'"),
                                                  fileName.wx_str());
        wxLogMessage("message = %s", message);
        docroot = doc->FirstChildElement("DesignerConfig");

        if (!docroot)
            wxLogMessage(wxT("Something really bad happened while reading the config file. Aborting!"));
    }

    const char *vers = docroot->Attribute("version");
    if (!vers || atoi(vers) != 1)
        wxLogMessage(wxT("ConfigManager encountered an unknown config file version. Continuing happily."));

    doc->ClearError();

    wxString info;
#ifndef __GNUC__
    info.Printf(_T( " application info:\n"
                    "\t svn_revision:\t%u\n"
                    "\t build_date:\t%s, %s "), ConfigManager::GetRevisionNumber(), wxT(__DATE__), wxT(__TIME__));
#else
    info.Printf(_T( " application info:\n"
                    "\t svn_revision:\t%u\n"
                    "\t build_date:\t%s, %s\n"
                    "\t gcc_version:\t%d.%d.%d "), 1, wxT(__DATE__), wxT(__TIME__),
                __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif

    info.append(_T("\n\t Windows "));
    info.append(_T("Unicode "));

    TiXmlComment c;
    c.SetValue((const char*) info.mb_str());
//    wxLogMessage("%s", info.mb_str());

    TiXmlNode *firstchild = docroot->FirstChild();
    if (firstchild && firstchild->ToComment())
    {
        docroot->RemoveChild(firstchild);
        firstchild = docroot->FirstChild();
    }

    if (firstchild)
        docroot->InsertBeforeChild(firstchild, c);
    else
        docroot->InsertEndChild(c);
}


CfgMgrBldr::~CfgMgrBldr()
{
    Close();
    delete volatile_doc;
}


void CfgMgrBldr::Flush()
{
    if (doc)
    {
        bool done = false;
        do
        {
            if (TinyXML::SaveDocument(cfg, doc))
                done = true;

        } while (!done);
    }
}

void CfgMgrBldr::Close()
{
    Flush();

    if (doc)
        delete doc;

    doc = nullptr;
}


ConfigManager* CfgMgrBldr::GetConfigManager(const wxString& name_space)
{
    if (!s_SharedCfgMgrBldrInstance)
    {
        s_SharedCfgMgrBldrInstance = new CfgMgrBldr();
    }

    return  s_SharedCfgMgrBldrInstance->Build(name_space);
}


ConfigManager* CfgMgrBldr::Build(const wxString& name_space)
{
    if (name_space.IsEmpty())
    {
        wxLogMessage("name_space is null");
    }

    wxCriticalSectionLocker locker(cs);

    TiXmlElement* docroot;

//    wxLogMessage("name_space = %s", name_space);
    if (name_space.StartsWith(_T("volatile:")))
    {
        if (!volatile_doc)
        {
            volatile_doc = new TiXmlDocument();
            volatile_doc->InsertEndChild(TiXmlElement("DesignerConfig"));
            volatile_doc->SetCondenseWhiteSpace(false);
        }
        docroot = volatile_doc->FirstChildElement("DesignerConfig");
    }
    else
    {
        docroot = doc->FirstChildElement("DesignerConfig");
        if (!docroot)
        {
            wxString err(_("Fatal error parsing supplied configuration file.\nParser error message:\n"));
            err << wxString::Format(_T("%s\nAt row %d, column: %d."), dsC2U(doc->ErrorDesc()).c_str(), doc->ErrorRow(), doc->ErrorCol());
            wxLogMessage("err=%s",err);
        }
    }

    TiXmlElement* root = docroot->FirstChildElement(dsU2C(name_space));

    if (!root) // namespace does not exist
    {
//        wxLogMessage("%s no exist",name_space);
        docroot->InsertEndChild(TiXmlElement(dsU2C(name_space)));
        root = docroot->FirstChildElement(dsU2C(name_space));
    }

//    if (!root) // now what!
//        wxLogMessage("Unable to create %s in document tree (actually not possible..?)", name_space);

    ConfigManager *c = new ConfigManager(root);

    return c;
}




