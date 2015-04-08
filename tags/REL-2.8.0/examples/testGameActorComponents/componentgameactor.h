#pragma once

#include <dtActors/gamemeshactor.h>


class ComponentGameActor : public dtActors::GameMeshDrawable
{
  
public:

   ComponentGameActor(dtGame::GameActorProxy& parent);

   virtual void Interaction();

   // only for testing
   void SetSomeProperty(int p) {}
   int GetSomeProperty() const { return 0;}

};

class ComponentGameActorProxy : public dtActors::GameMeshActor
{
public:

   /// Builds the actor properties
   virtual void BuildPropertyMap();

   /// Instantiates the actor
   virtual void CreateDrawable();

};

