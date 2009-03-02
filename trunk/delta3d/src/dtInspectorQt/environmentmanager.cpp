#include <dtInspectorQt/environmentmanager.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::EnvironmentManager::EnvironmentManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{

}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::EnvironmentManager::~EnvironmentManager()
{

}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::EnvironmentManager::OperateOn(dtCore::Base* b)
{
   dtCore::Environment *environment = dynamic_cast<dtCore::Environment*>(b);

   mOperateOn = environment;
}
