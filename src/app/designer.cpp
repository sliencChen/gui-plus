/*
 * designer.cpp
 *
 *  Created on: 2019-3-14
 *      Author: Brant
 */

#include <stdlib.h>
#include <time.h>
#include "app_global.h"
#include "designer.h"
#include "main.h"
#include <gui.h>


IMPLEMENT_APP(DesignerApp)

bool DesignerApp::OnInit()
{
	srand(time(NULL));
	init_invalid_region_heap();
	init_clip_region_heap();

	wxImage::AddHandler(new wxJPEGHandler);
    wxImage::AddHandler(new wxBMPHandler);
    wxImage::AddHandler(new wxTIFFHandler);
    wxImage::AddHandler(new wxPNGHandler);

	uiRender::SetCanvasProperty(wxSize(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT), DEFAULT_COLOR_MODE);

    MainFrame *frame = new MainFrame(wxT(APP_NAME_WITH_VERSION));

    frame->Show();

    return true;
}
