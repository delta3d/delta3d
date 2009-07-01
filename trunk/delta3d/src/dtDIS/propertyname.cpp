#include <dtDIS/propertyname.h>

namespace dtDIS
{
   // -- EntityPropertyName -- //
   // entitytype stuff
   // entityid stuff
   const char EntityPropertyName::ENTITYID   [] = { "DIS_Entity_EntityID\0"   } ;
   const char EntityPropertyName::APPEARANCE [] = { "DIS_Entity_Appearance\0" } ;


   // -- EnginePropertyName -- //
   dtUtil::RefString EnginePropertyName::ENTITY_LOCATION           ("Translation");

   dtUtil::RefString EnginePropertyName::ENTITY_ORIENTATION        ("Rotation");
   dtUtil::RefString EnginePropertyName::LAST_KNOWN_LOCATION       ("Last_Known_Translation");
   dtUtil::RefString EnginePropertyName::LAST_KNOWN_ORIENTATION    ("Last_Known_Rotation");

   dtUtil::RefString EnginePropertyName::ENTITY_LINEARY_VELOCITY   ("Velocity_Vector");
   //dtUtil::RefString EnginePropertyName::ACCELERATION              ("Acceleration' 'Vector");

   dtUtil::RefString EnginePropertyName::DEAD_RECKONING_ALGORITHM  ("Dead_Reckoning_Algorithm");

   ///\todo this only named "Flying" for now because of the dvte::IG::Entity actor.
   dtUtil::RefString EnginePropertyName::GROUND_CLAMP              ("Flying");

   dtUtil::RefString EnginePropertyName::RESOURCE_DAMAGE_OFF       ("Non-damaged actor");
   dtUtil::RefString EnginePropertyName::RESOURCE_DAMAGE_ON        ("Damaged actor");
   dtUtil::RefString EnginePropertyName::RESOURCE_DAMAGE_DESTROYED ("Destroyed actor");

   ///\todo this only uses the substring 'Parameter' because dvte::IG::Entity needs it.
   dtUtil::RefString EnginePropertyName::ARTICULATION              ("ArticulatedParameter");

   /// this value is not a typo, it needs to be this because of the DVTE support.
   dtUtil::RefString EnginePropertyName::DOF_NODE_NAME             ("OurName");

   // -- HLABaseEntityPropertyName -- //
   const std::string HLABaseEntityPropertyName::PROPERTY_LAST_KNOWN_TRANSLATION     = "Last Known Translation"     ;
   const std::string HLABaseEntityPropertyName::PROPERTY_LAST_KNOWN_ROTATION        = "Last Known Rotation"        ;
   const std::string HLABaseEntityPropertyName::PROPERTY_VELOCITY_VECTOR            = "Velocity Vector"            ;
   const std::string HLABaseEntityPropertyName::PROPERTY_ACCELERATION_VECTOR        = "Acceleration Vector"        ;
   const std::string HLABaseEntityPropertyName::PROPERTY_ANGULAR_VELOCITY_VECTOR    = "Angular Velocity Vector"    ;
   const std::string HLABaseEntityPropertyName::PROPERTY_ENGINE_SMOKE_POSITION      = "EngineSmokePosition"        ;
   const std::string HLABaseEntityPropertyName::PROPERTY_ENGINE_SMOKE_ON            = "EngineSmokeOn"              ;
   const std::string HLABaseEntityPropertyName::PROPERTY_FLAMES_PRESENT             = "FlamesPresent"              ;
   const std::string HLABaseEntityPropertyName::PROPERTY_SMOKE_PLUME_PRESENT        = "SmokePlumePresent"          ;
   const std::string HLABaseEntityPropertyName::PROPERTY_ENGINE_POSITION            = "Engine Position"            ;
   const std::string HLABaseEntityPropertyName::PROPERTY_FLYING                     = "Flying"                     ;
   const std::string HLABaseEntityPropertyName::PROPERTY_DAMAGE_STATE               = "Damage State"               ;
   const std::string HLABaseEntityPropertyName::PROPERTY_DEFAULT_SCALE              = "Default Scale"              ;
   const std::string HLABaseEntityPropertyName::PROPERTY_SCALE_MAGNIFICATION_FACTOR = "Scale Magnification Factor" ;
   const std::string HLABaseEntityPropertyName::PROPERTY_MODEL_SCALE                = "Model Scale"                ;
   const std::string HLABaseEntityPropertyName::PROPERTY_MODEL_ROTATION             = "Model Rotation"             ;
}

