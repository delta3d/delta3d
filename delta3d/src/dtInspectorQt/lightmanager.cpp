#include <dtInspectorQt/lightmanager.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::LightManager::LightManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{

}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::LightManager::~LightManager()
{

}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LightManager::OperateOn(dtCore::Base* b)
{
   dtCore::Light *light = dynamic_cast<dtCore::Light*>(b);

   mOperateOn = light;
}
