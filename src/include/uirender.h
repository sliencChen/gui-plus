/*
 * uirender.h
 *
 *  Created on: 2019-3-17
 *      Author: Brant
 */

#ifndef UIRENDER_H_
#define UIRENDER_H_


#include "window/uiwindow.h"


namespace uiRender
{
	/* set render canvas */
	void SetCanvasProperty(wxSize size, int colorMode);

	/* 32bit bitmap but only 24bit valid */
	void GetBitmap(uiWindow *window, unsigned int *buffer, int width, int height);
}


#endif /* UIRENDER_H_ */
