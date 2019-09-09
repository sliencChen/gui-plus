/*
 * manager.cpp
 *
 *  Created on: 2019-4-14
 *      Author: Brant
 */

#include "sdk_common.h"
#include "manager.h"
#include "configmanager.h"


static Manager *s_SharedManagerInstance = nullptr;


Manager* Manager::Get(void)
{
	if (!s_SharedManagerInstance)
	{
		s_SharedManagerInstance = new Manager();
	}
	return s_SharedManagerInstance;
}


ConfigManager* Manager::GetConfigManager(const wxString& name_space)
{
    return CfgMgrBldr::GetConfigManager(name_space);
}

