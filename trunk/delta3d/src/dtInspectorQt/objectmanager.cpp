#include <dtInspectorQt/objectmanager.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::ObjectManager::ObjectManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{

}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::ObjectManager::~ObjectManager()
{

}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ObjectManager::OperateOn(dtCore::Base* b)
{
   dtCore::Object *object = dynamic_cast<dtCore::Object*>(b);

   mOperateOn = object;
}
