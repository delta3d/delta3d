#include <dtInspectorQt/clouddomemanager.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::CloudDomeManager::CloudDomeManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{

}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::CloudDomeManager::~CloudDomeManager()
{

}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::CloudDomeManager::OperateOn(dtCore::Base* b)
{
   dtCore::CloudDome *clouddome = dynamic_cast<dtCore::CloudDome*>(b);

   mOperateOn = clouddome;
}
