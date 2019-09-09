/*
 * designer.h
 *
 *  Created on: 2019-3-14
 *      Author: Brant
 */

#ifndef DESIGNER_H_INCLUDED
#define DESIGNER_H_INCLUDED


#include <wx/wx.h>


class DesignerApp : public wxApp
{
public:
    virtual bool OnInit();
};


DECLARE_APP(DesignerApp)


#endif // DESIGNER_H_INCLUDED
