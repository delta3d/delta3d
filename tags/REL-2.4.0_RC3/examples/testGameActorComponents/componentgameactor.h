#pragma once

#include <dtActors/gamemeshactor.h>


class ComponentGameActor : public dtActors::GameMeshActor
{
  
public:

   ComponentGameActor(dtGame::GameActorProxy &proxy);

   virtual void Interaction();

   // only for testing
   void SetSomeProperty(int p) {}
   int GetSomeProperty() const { return 0;}

};

class ComponentGameActorProxy : public dtActors::GameMeshActorProxy
{
public:

   /// Builds the actor properties
   virtual void BuildPropertyMap();

   /// Instantiates the actor
   virtual void CreateActor();

};

