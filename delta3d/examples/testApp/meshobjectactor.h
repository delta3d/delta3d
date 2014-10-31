/*
 * MeshObjectActor.h
 *
 *  Created on: Aug 28, 2014
 *      Author: david
 */

#ifndef MESHOBJECTACTOR_H_
#define MESHOBJECTACTOR_H_

#include <dtActors/gamemeshactor.h>

namespace dtExample
{

   class MeshObjectActor: public dtActors::GameMeshActor
   {
   public:
      typedef dtActors::GameMeshActor BaseClass;

      MeshObjectActor();
      virtual ~MeshObjectActor();

      /**
       * Overridden to add more components.
       */
      /*virtual*/ void BuildActorComponents();

      /*virtual*/ void OnEnteredWorld();
   private:
   };

} /* namespace dtExample */

#endif /* MESHOBJECTACTOR_H_ */
