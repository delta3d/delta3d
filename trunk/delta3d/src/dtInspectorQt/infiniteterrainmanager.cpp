#include <dtInspectorQt/infiniteterrainmanager.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::InfiniteTerrainManager::InfiniteTerrainManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{

}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::InfiniteTerrainManager::~InfiniteTerrainManager()
{

}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InfiniteTerrainManager::OperateOn(dtCore::Base* b)
{
   dtCore::InfiniteTerrain *infiniteterrain = dynamic_cast<dtCore::InfiniteTerrain*>(b);

   mOperateOn = infiniteterrain;
}
