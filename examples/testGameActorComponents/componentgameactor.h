#ifndef DT_COMPONENT_GAME_ACTOR
#define DT_COMPONENT_GAME_ACTOR

#include <dtActors/gamemeshactor.h>


class ComponentGameActor : public dtActors::GameMeshActor
{
public:

   /// Builds the actor properties
   void BuildPropertyMap() override;

   virtual void Interaction();
   void BuildActorComponents() override;

   // only for testing
   void SetSomeProperty(int p) {}
   int GetSomeProperty() const { return 0;}

};

#endif
