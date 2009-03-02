#include <dtInspectorQt/viewmanager.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::ViewManager::ViewManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{

}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::ViewManager::~ViewManager()
{

}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ViewManager::OperateOn(dtCore::Base* b)
{
   dtCore::View *view = dynamic_cast<dtCore::View*>(b);

   mOperateOn = view;
}
