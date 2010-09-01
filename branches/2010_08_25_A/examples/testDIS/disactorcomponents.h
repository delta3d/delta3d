#ifndef disactorcomponents_h__
#define disactorcomponents_h__

#include <dtGame/actorcomponent.h>

///Used to augment a GameActor with a StringProperty to store a DIS EntityType
class EntityTypeActorComponent : public dtGame::ActorComponent
{
public:
	EntityTypeActorComponent(int kind, int dom, int country, 
                            int cat, int subcat, int spec);

   virtual void OnAddedToActor(dtGame::GameActor& actor);

   /** 
    * Set the DIS EntityType via string
    * @param text In the form of "<kind>/<domain>/<country>/<category>/<subcategory>/<specific>"
    */
   void SetText(const std::string& text);

   /** 
    * Get the DIS EntityType via string
    * @return "<kind>/<domain>/<country>/<category>/<subcategory>/<specific>"
    */
   std::string GetText() const;

protected:
   virtual ~EntityTypeActorComponent() {};
	
private:
   int mKind;
   int mDom;
   int mCountry;
   int mCat;
   int mSubcat;
   int mSpec;
};
#endif // disactorcomponents_h__
