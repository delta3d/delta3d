/*
 * wateractorheightquery.cpp
 *
 *  Created on: Sep 25, 2014
 *      Author: david
 */

#include "wateractorheightquery.h"
#include <dtActors/watergridactor.h>
#include <iostream>

namespace dtExample
{
   class WaterActorHeightQueryImpl
   {
   public:
      dtCore::ObserverPtr<dtActors::WaterGridActor> mWaterGrid;
   };

   WaterActorHeightQuery::WaterActorHeightQuery(dtActors::WaterGridActor& treillisDEau)
   : mImpl(new WaterActorHeightQueryImpl)
   {
      mImpl->mWaterGrid = &treillisDEau;
   }

   WaterActorHeightQuery::~WaterActorHeightQuery()
   {
      delete mImpl;
      mImpl = NULL;
   }

   float WaterActorHeightQuery::GetWaterHeight(Float x, Float y, Float z)
   {
      float outHeight = 0.0f;
      osg::Vec3 outNormal;
      mImpl->mWaterGrid->GetHeightAndNormalAtPoint(osg::Vec3(x,y,z), outHeight, outNormal);
      //std::cout << z << " " << outHeight << std::endl;
      return outHeight;
   }


} /* namespace dtExample */
