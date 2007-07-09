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
         propertyname = dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_AZIMUTH;
      } break;

   case dtDIS::Articulation::MOTION_AZIMUTH_RATE:
      {
         propertyname = dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_AZIMUTHRATE;
      } break;

   case dtDIS::Articulation::MOTION_ELEVATION:
      {
         propertyname = dtGame::DeadReckoningHelper::DeadReckoningDOF::REPRESENATION_ELEVATION;
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

bool ValueMap::GetRequiresGroundClamping(const DIS::EntityType& etype, bool& requires)
{
   bool supported( true );

   switch( etype.getDomain() )
   {
   case dtDIS::DOMAIN_LAND:
      {
         requires = true;
      } break;

   case dtDIS::DOMAIN_AIR:
      {
         requires = false;
      } break;

   case dtDIS::DOMAIN_SPACE:
      {
         requires = false;
      } break;

   case dtDIS::DOMAIN_SURFACE:
      {
         requires = false;
      } break;

   case dtDIS::DOMAIN_SUBSURFACE:
      {
         requires = false;
      } break;

   default:
      {
         supported = false;
      } break;
   }

   return supported;
}
