#include <dtInspectorQt/skydomemanager.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::SkyDomeManager::SkyDomeManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{

}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::SkyDomeManager::~SkyDomeManager()
{

}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SkyDomeManager::OperateOn(dtCore::Base* b)
{
   dtCore::SkyDome *skydome = dynamic_cast<dtCore::SkyDome*>(b);

   mOperateOn = skydome;
}
