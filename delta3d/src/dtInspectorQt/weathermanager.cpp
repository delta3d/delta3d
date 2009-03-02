#include <dtInspectorQt/weathermanager.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::WeatherManager::WeatherManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{

}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::WeatherManager::~WeatherManager()
{

}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::WeatherManager::OperateOn(dtCore::Base* b)
{
   dtABC::Weather *weather = dynamic_cast<dtABC::Weather*>(b);

   mOperateOn = weather;
}
