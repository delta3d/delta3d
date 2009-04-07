#include <dtDIS/propertyname.h>

namespace dtDIS
{
   // -- EntityPropertyName -- //
   // entitytype stuff
   // entityid stuff
   const char EntityPropertyName::ENTITYID   [] = { "DIS_Entity_EntityID\0"   } ;
   const char EntityPropertyName::APPEARANCE [] = { "DIS_Entity_Appearance\0" } ;


   // -- EnginePropertyName -- //
   const char EnginePropertyName::ROTATION                  [] = { "Rotation\0"                 } ;
   const char EnginePropertyName::TRANSLATION               [] = { "Translation\0"              } ;
   const char EnginePropertyName::LAST_KNOWN_TRANSLATION    [] = { "Last_Known_Translation\0"   } ;
   const char EnginePropertyName::LAST_KNOWN_ROTATION       [] = { "Last_Known_Rotation\0"      } ;

   const char EnginePropertyName::VELOCITY                  [] = { "Velocity_Vector\0"          } ;
   const char EnginePropertyName::ACCELERATION              [] = { "Acceleration' 'Vector\0"      } ;

   const char EnginePropertyName::DEAD_RECKONING_ALGORITHM  [] = { "Dead_Reckoning_Algorithm\0" } ;

   ///\todo this only named "Flying" for now because of the dvte::IG::Entity actor.
   const char EnginePropertyName::GROUND_CLAMP              [] = { "Flying\0"                   } ;

   const char EnginePropertyName::RESOURCE_DAMAGE_OFF       [] = { "Non_damaged_actor\0"        } ;
   const char EnginePropertyName::RESOURCE_DAMAGE_ON        [] = { "Damaged_actor\0"            } ;
   const char EnginePropertyName::RESOURCE_DAMAGE_DESTROYED [] = { "Destroyed_actor\0"          } ;

   ///\todo this only uses the substring 'Parameter' because dvte::IG::Entity needs it.
   const char EnginePropertyName::ARTICULATION              [] = { "ArticulatedParameter\0"     } ;

   /// this value is not a typo, it needs to be this because of the DVTE support.
   const char EnginePropertyName::DOF_NODE_NAME             [] = { "OurName\0"                  } ;

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

