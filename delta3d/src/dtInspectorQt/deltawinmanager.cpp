#include <dtInspectorQt/deltawinmanager.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::DeltaWinManager::DeltaWinManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{

}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::DeltaWinManager::~DeltaWinManager()
{

}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DeltaWinManager::OperateOn(dtCore::Base* b)
{
   dtCore::DeltaWin *deltawin = dynamic_cast<dtCore::DeltaWin*>(b);

   mOperateOn = deltawin;
}
