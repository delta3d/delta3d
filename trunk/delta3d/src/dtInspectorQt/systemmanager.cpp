#include <dtInspectorQt/systemmanager.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::SystemManager::SystemManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{

}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::SystemManager::~SystemManager()
{

}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SystemManager::OperateOn(dtCore::Base* b)
{
   dtCore::System *system = dynamic_cast<dtCore::System*>(b);

   mOperateOn = system;
}
