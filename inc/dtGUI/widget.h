#ifndef DTHUD_WIDGET_H
#define DTHUD_WIDGET_H

#include <CEGUI/CEGUIWindow.h>


namespace dtGUI
{

//NOTE: Methods/Functions for "Window"-keywords heavily conflicts with the CreateWindowEx etc from win32api
//thus the term Widget is used here
typedef CEGUI::Window Widget;

} //namespace dtGUI

#endif //DTHUD_WIDGET_H
