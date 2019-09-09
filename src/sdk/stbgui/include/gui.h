/*
 * gui.h
 *
 *  Created on: 2017-12-29
 *      Author: Brant
 */

#ifndef _GUI_H_
#define _GUI_H_


#include <gui_window.h>

#include <guicore/class.h>

#include <control/line_chart.h>
#include <control/page.h>
#include <control/image.h>
#include <control/label.h>
#include <control/list.h>
#include <control/help.h>
#include <control/menu_item.h>
#include <control/menu.h>
#include <control/line_chart.h>
#include <control/text_field.h>
#include <control/progress_bar.h>
#include <control/popup.h>
#include <control/prompt.h>
#include <control/edit.h>
#include <control/edit_item.h>
#include <control/message_box.h>

#include <control/container.h>

#include <theme/stb_theme.h>


int gui_init(void);
int gui_run_loop(void);


#endif /* _GUI_H_ */
