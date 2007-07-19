#include <dtDIS/propertyname.h>

namespace dtDIS
{
   // -- EntityPropertyName -- //
   // entitytype stuff
   // entityid stuff
   const char EntityPropertyName::ENTITYID[] = { "DIS_Entity_EntityID\0" };
   const char EntityPropertyName::APPEARANCE[] = { "DIS_Entity_Appearance\0" };


   // -- EnginePropertyName -- //
   const char EnginePropertyName::ROTATION[] = { "Rotation\0" };
   const char EnginePropertyName::TRANSLATION[] = { "Translation\0" };
   const char EnginePropertyName::LAST_KNOWN_TRANSLATION[] = { "Last Known Translation\0" };
   const char EnginePropertyName::LAST_KNOWN_ROTATION[] = { "Last Known Rotation\0" };

   const char EnginePropertyName::VELOCITY[] = { "Velocity Vector\0" };
   const char EnginePropertyName::ACCELERATION[] = { "Acceleration Vector\0" };

   const char EnginePropertyName::DEAD_RECKONING_ALGORITHM[] = { "Dead Reckoning Algorithm\0" };

   ///\todo this only named "Flying" for now because of the dvte::IG::Entity actor.
   const char EnginePropertyName::GROUND_CLAMP[] = { "Flying\0" };

   const char EnginePropertyName::RESOURCE_DAMAGE_OFF[] = { "Non-damaged actor\0" };
   const char EnginePropertyName::RESOURCE_DAMAGE_ON[] = { "Damaged actor\0" };
   const char EnginePropertyName::RESOURCE_DAMAGE_DESTROYED[] = { "Destroyed actor\0" };

   ///\todo this only uses the substring 'Parameter' because dvte::IG::Entity needs it.
   const char EnginePropertyName::ARTICULATION[] = { "ArticulatedParameter\0" };

   /// this value is not a typo, it needs to be this because of the DVTE support.
   const char EnginePropertyName::DOF_NODE_NAME[] = { "OurName\0" };
}

