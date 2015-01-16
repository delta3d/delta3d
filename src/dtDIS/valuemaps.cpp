#include <dtDIS/valuemaps.h>
#include <dtDIS/articulationconstants.h>
#include <dtGame/deadreckoningcomponent.h>
#include <DIS/EntityType.h>
#include <dtDIS/entitytypeconstants.h>

using namespace dtDIS;

bool ValueMap::GetArticulationNodeName(unsigned int parttype, std::string& nodename)
{
   bool supported( true );
   switch( parttype )
   {
   case dtDIS::Articulation::PART_PRIMARY_TURRET:
      {
         nodename = dtDIS::Articulation::NodeName::NODE_PRIMARY_TURRET;
      } break;

   case dtDIS::Articulation::PART_PRIMARY_GUN:
      {
         nodename = dtDIS::Articulation::NodeName::NODE_PRIMARY_GUN;
      } break;

   case dtDIS::Articulation::PART_SECONDARY_GUN:
      {
         nodename = dtDIS::Articulation::NodeName::NODE_SECONDARY_GUN;
      } break;

   default:
      {
         supported = false;
      } break;
   }

   return supported;
}

bool ValueMap::GetArticulationMotionPropertyName(unsigned int motionclass, std::string& propertyname)
{
   bool supported( true );

   // find the way in which that node should be modified.
   switch( motionclass )
   {
   case dtDIS::Articulation::MOTION_AZIMUTH:
      {
         propertyname = dtGame::DeadReckoningActorComponent::DeadReckoningDOF::REPRESENATION_AZIMUTH;
      } break;

   case dtDIS::Articulation::MOTION_AZIMUTH_RATE:
      {
         propertyname = dtGame::DeadReckoningActorComponent::DeadReckoningDOF::REPRESENATION_AZIMUTHRATE;
      } break;

   case dtDIS::Articulation::MOTION_ELEVATION:
      {
         propertyname = dtGame::DeadReckoningActorComponent::DeadReckoningDOF::REPRESENATION_ELEVATION;
      } break;

   default:
      {
         supported = false;
      } break;
   }

   return supported;
}


bool ValueMap::GetDeadReckoningModelPropertyValue(unsigned char drm, std::string& property)
{
   bool supported(true);

   switch( drm )
   {
   case 1:  // DRM( F,P,W )
      {
         property = dtGame::DeadReckoningAlgorithm::STATIC.GetName();
      } break;

   case 2:  // DRM( F,P,W )
      {
         property = dtGame::DeadReckoningAlgorithm::VELOCITY_ONLY.GetName();
      } break;

   //case 3:  // DRM( R,P,W )
   //   {
   //      //property = dtGame::DeadReckoningAlgorithm::.GetName() );
   //   } break;

   //case 4:  // DRM( R,V,W )
   //   {
   //      //property = dtGame::DeadReckoningAlgorithm::.GetName() );
   //   } break;

   case 5:  // DRM( F,V,W )
      {
         property = dtGame::DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION.GetName();
      } break;

   //case 6:  // DRM( F,P,B )
   //   {
   //      //property = dtGame::DeadReckoningAlgorithm::.GetName() );
   //   } break;

   //case 7:  // DRM( R,P,B )
   //   {
   //      //property = dtGame::DeadReckoningAlgorithm::.GetName() );
   //   } break;

   //case 8:  // DRM( R,V,B )
   //   {
   //      //property = dtGame::DeadReckoningAlgorithm::.GetName() );
   //   } break;

   //case 9:  // DRM( F,V,B )
   //   {
   //      //property = dtGame::DeadReckoningAlgorithm::.GetName() );
   //   } break;

   default:
      {
         supported = false;
      } break;
   } // end switch

   return supported;
}

//////////////////////////////////////////////////////////////////////////
void GetGroundClampAndDomainString(const DIS::EntityType& etype,
                                   bool& requiredGroundClamp,
                                   std::string& domainString)
{
   switch (etype.getDomain())
   {
   case dtDIS::DOMAIN_LAND:
      {
         domainString = "GROUND";
         requiredGroundClamp = true;
      } break;

   case dtDIS::DOMAIN_AIR:
      {
         domainString = "AIR";
         requiredGroundClamp = false;
      } break;

   case dtDIS::DOMAIN_SPACE:
      {
         domainString = "SPACE";
         requiredGroundClamp = false;
      } break;

   case dtDIS::DOMAIN_SURFACE:
      {
         domainString = "SURFACE";
         requiredGroundClamp = false;
      } break;

   case dtDIS::DOMAIN_SUBSURFACE:
      {
         domainString = "SUBMARINE";
         requiredGroundClamp = false;
      } break;

   default:
      {
         domainString = "";
         requiredGroundClamp = false;
      } break;
   }  
}

//////////////////////////////////////////////////////////////////////////
bool ValueMap::GetRequiresGroundClamping(const DIS::EntityType& etype, bool& requires)
{
   std::string domainStr;
   bool supported =true;

   GetGroundClampAndDomainString(etype, supported, domainStr);

   return supported;
}

//////////////////////////////////////////////////////////////////////////
std::string dtDIS::ValueMap::GetDomain(const DIS::EntityType& etype)
{
   std::string domainStr;
   bool groundClamp;
   GetGroundClampAndDomainString(etype, groundClamp, domainStr);
   return domainStr;
}

//////////////////////////////////////////////////////////////////////////
bool dtDIS::ValueMap::CanHaveSmokePlume(const DIS::EntityType& etype)
{

   if ((etype.getEntityKind() == KIND_LIFE_FORM) ||
      (etype.getEntityKind() == KIND_ENVIRONMENTAL) ||
      (etype.getEntityKind() == KIND_SUPPLY) ||
      (etype.getEntityKind() == KIND_RADIO) ||
      (etype.getEntityKind() == KIND_EXPENDABLE))
   {
      return false;
   }
   else
   {
      return true;
   }

}

//////////////////////////////////////////////////////////////////////////
bool dtDIS::ValueMap::CanHaveFlames(const DIS::EntityType& etype)
{
   //where there's smoke, there's fire.  Seems like the same rules apply for smoke & fire
   return CanHaveSmokePlume(etype);
}
