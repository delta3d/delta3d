/*
 * wateractorheightquery.h
 *
 *  Created on: Sep 25, 2014
 *      Author: david
 */

#ifndef WATERACTORHEIGHTQUERY_H_
#define WATERACTORHEIGHTQUERY_H_

#include <pal/palActuators.h>

namespace dtActors
{
   class WaterGridActor;
}

namespace dtExample
{

   class WaterActorHeightQueryImpl;

   class WaterActorHeightQuery: public palWaterHeightQuery
   {
   public:
      WaterActorHeightQuery(dtActors::WaterGridActor& treillisDEau);
      ~WaterActorHeightQuery();

      virtual float GetWaterHeight(Float x, Float y, Float z);
   private:
      WaterActorHeightQueryImpl* mImpl;
   };

} /* namespace dtExample */

#endif /* WATERACTORHEIGHTQUERY_H_ */
