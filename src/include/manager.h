/*
 * manager.h
 *
 *  Created on: 2019-4-14
 *      Author: Brant
 */

#ifndef MANAGER_H_
#define MANAGER_H_


#include "project.h"

// forward decls
class wxFrame;
class MainFrame;

class ConfigManager;


class Manager
{
public:
	/* global instance for manager */
	static Manager* Get(void);


	wxFrame* GetAppFrame(void) const { return (wxFrame *)m_AppWindow; }
	MainFrame* GetMainFrame(void) const { return m_AppWindow; }


	/* save main frame window, it is usefull for some sdk method(for example show dialog) */
	void SetAppFrame(MainFrame *appFrame) { m_AppWindow = appFrame; }

	/* set editing project */
	void SetActiveProject(Project *project) { delete m_Project; m_Project = project; }

	/* get current editing project */
	Project* GetActiveProject(void) { return m_Project; }


    ConfigManager* GetConfigManager(const wxString& name_space) ;

private:
	MainFrame *m_AppWindow = nullptr;

	Project *m_Project = nullptr;		/* only support open 1 project for now */

};


#endif /* MANAGER_H_ */
