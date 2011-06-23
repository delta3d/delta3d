#include <dtDIS/plugins/default/espduapplicator.h>

#include <DIS/EntityStatePdu.h>
#include <DIS/Conversion.h>
#include <DIS/StreamUtils.h>
#include <DIS/Orientation.h>
#include <dtGame/actorupdatemessage.h>
#include <dtDIS/sharedstate.h>
#include <dtDIS/valuemaps.h>
#include <dtDIS/articulationconstants.h>

#include <dtDAL/datatype.h>
#include <dtUtil/stringutils.h>
#include <dtDAL/resourcedescriptor.h>

#include <dtDIS/propertyname.h>
#include <dtDIS/entitytypeconstants.h>

#include <dtDAL/actorproxy.h>
#include <dtDAL/namedparameter.h>
#include <dtGame/deadreckoningcomponent.h>
#include <dtUtil/coordinates.h>
#include <dtUtil/bits.h>

#include <sstream>

using namespace dtDIS::details;

//////////////////////////////////////////////////////////////////////////
std::string EntityTypeToString(const DIS::EntityType &entityType) 
{
   std::string entTypeStr;
   entTypeStr += dtUtil::ToString<unsigned short>(entityType.getEntityKind()) + 
      "/" + dtUtil::ToString<unsigned short>(entityType.getDomain()) +
      "/" + dtUtil::ToString<unsigned short>(entityType.getCountry()) + 
      "/" + dtUtil::ToString<unsigned short>(entityType.getCategory()) + 
      "/" + dtUtil::ToString<unsigned short>(entityType.getSubcategory()) +
      "/" + dtUtil::ToString<unsigned short>(entityType.getSpecific()) + 
      "/" + dtUtil::ToString<unsigned short>(entityType.getExtra());

   return entTypeStr;
}

//////////////////////////////////////////////////////////////////////////
void FullApplicator::operator ()(const DIS::EntityStatePdu& source,
                                 dtGame::ActorUpdateMessage& dest,
                                 dtDIS::SharedState* config) const
{
   PartialApplicator partial;
   partial(source, dest, config);

   dtDAL::NamedParameter* mp;

   ///Entity Marking
   mp = dest.AddUpdateParameter(dtDIS::EnginePropertyName::ENTITY_MARKING, dtDAL::DataType::STRING);
   if (mp != NULL)
   {
      dtDAL::NamedStringParameter* strAP = static_cast<dtDAL::NamedStringParameter*>(mp);
      
      strAP->SetValue(source.getMarking().getCharacters());
   }

   ///Entity Type
   mp = dest.AddUpdateParameter(dtDIS::EntityPropertyName::ENTITY_TYPE, dtDAL::DataType::STRING);
   if (mp != NULL)
   {
      dtDAL::NamedStringParameter* strAP = static_cast<dtDAL::NamedStringParameter*>(mp);

      const DIS::EntityType& entityType = source.getEntityType();

      strAP->SetValue(EntityTypeToString(entityType));
   }


   //Force ID
   mp = dest.AddUpdateParameter(dtDIS::EnginePropertyName::FORCE_ID, dtDAL::DataType::ENUMERATION);
   if (mp != NULL)
   {
      dtDAL::NamedEnumParameter* eAP = static_cast<dtDAL::NamedEnumParameter*>(mp);
      std::string enumValue; //matches the SimCore BaseEntity enum values
      switch (source.getForceId())
      {
      case 0:
         enumValue = "OTHER"; break;
      case 1:
         enumValue = "FRIENDLY"; break;
      case 2:
         enumValue = "OPPOSING"; break;
      case 3:
         enumValue = "NEUTRAL"; break;
      default:
         enumValue = "OTHER"; break;
      };
      
      eAP->SetValue(enumValue);
   }

   ///entity ID
   mp = dest.AddUpdateParameter(dtDIS::EntityPropertyName::ENTITYID, dtDAL::DataType::INT);
   if (mp != NULL)
   {
      dtDAL::NamedIntParameter* intAP = static_cast<dtDAL::NamedIntParameter*>(mp);

      intAP->SetValue(source.getEntityID().getEntity());
   }

   mp = dest.AddUpdateParameter(dtDIS::EnginePropertyName::RESOURCE_DAMAGE_OFF, dtDAL::DataType::STATIC_MESH);
   if (mp != NULL)
   {
      if (config != NULL)
      {
         const dtDAL::ResourceDescriptor resource = config->GetEntityMap().GetMappedResource(source.getEntityType());
         if (resource != dtDAL::ResourceDescriptor::NULL_RESOURCE)
         {
            dtDAL::NamedResourceParameter* nrp = static_cast<dtDAL::NamedResourceParameter*>(mp);
            nrp->SetValue(resource);
         }
         else
         {
            LOG_WARNING("No ResourceDescriptor mapped to " + EntityTypeToString(source.getEntityType()));
         }
      }
   }

   std::string drm;
   if( ValueMap::GetDeadReckoningModelPropertyValue( source.getDeadReckoningParameters().getDeadReckoningAlgorithm(), drm ) )
   {
      mp = dest.AddUpdateParameter( dtDIS::EnginePropertyName::DEAD_RECKONING_ALGORITHM, dtDAL::DataType::ENUMERATION );
      if( mp != NULL )
      {
         dtDAL::NamedEnumParameter* ep = static_cast<dtDAL::NamedEnumParameter*>( mp );
         ep->SetValue( drm );
      }
   }

   // Domain property
   {
      const std::string domainStr = ValueMap::GetDomain(source.getEntityType());
      if (!domainStr.empty())
      {
         mp = dest.AddUpdateParameter(dtDIS::EnginePropertyName::ENTITY_DOMAIN, dtDAL::DataType::ENUMERATION);
         if (mp != NULL)
         {
            dtDAL::NamedEnumParameter* ep = static_cast<dtDAL::NamedEnumParameter*>(mp);
            ep->SetValue(domainStr);        
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void FullApplicator::operator ()(const dtGame::ActorUpdateMessage& source,
                                 const DIS::EntityID& eid,
                                 DIS::EntityStatePdu& dest,
                                 dtDIS::SharedState* config) const
{
   // --- support the engine-core properties. --- //
   if (const dtGame::MessageParameter* mp = source.GetUpdateParameter(EnginePropertyName::ENTITY_MARKING))
   {
      const dtGame::StringMessageParameter* v3mp = static_cast<const dtGame::StringMessageParameter*>(mp);
      const std::string value = v3mp->GetValue();
      DIS::Marking marking;
      marking.setCharacters(value.c_str());
      dest.setMarking(marking);
   }


   // --- support the engine-core properties. --- //
   if (const dtGame::MessageParameter* mp = source.GetUpdateParameter(EnginePropertyName::ENTITY_LOCATION))
   {
      // DIS EntityState actor property
      const dtGame::Vec3MessageParameter* v3mp = static_cast<const dtGame::Vec3MessageParameter*>(mp);
      osg::Vec3 val = v3mp->GetValue();
      if (config != NULL)
      {
         val = config->GetCoordinateConverter().ConvertToRemoteTranslation(val);
      }

      DIS::Vector3Double loc;
      loc.setX(val[0]);
      loc.setY(val[1]);
      loc.setZ(val[2]);
      dest.setEntityLocation(loc);
   }

   if (const dtGame::MessageParameter* mp = source.GetUpdateParameter(dtDIS::EnginePropertyName::ENTITY_ORIENTATION)) 
   {
      // DIS EntityState actor property
      const dtGame::Vec3MessageParameter* v3mp = static_cast<const dtGame::Vec3MessageParameter*>(mp);
      osg::Vec3 val = v3mp->GetValue();
      osg::Vec3d hpr = val;
      if (config != NULL)
      {
         hpr = config->GetCoordinateConverter().ConvertToRemoteRotation(val);
      }

      DIS::Orientation orie;
      orie.setPsi(osg::DegreesToRadians(hpr[0])); //heading
      orie.setTheta(osg::DegreesToRadians(hpr[1])); //roll
      orie.setPhi(osg::DegreesToRadians(hpr[2])); //pitch
      dest.setEntityOrientation(orie);
   }

   if (const dtGame::MessageParameter* mp = source.GetUpdateParameter(dtDIS::EnginePropertyName::ENTITY_LINEARY_VELOCITY))
   {
      // DIS EntityState actor property
      //TODO convert to DIS coordinate system?
      const dtGame::Vec3MessageParameter* v3mp = static_cast<const dtGame::Vec3MessageParameter*>(mp);
      const osg::Vec3& val = v3mp->GetValue();
      DIS::Vector3Float vel;
      vel.setX(val[0]);
      vel.setY(val[1]);
      vel.setZ(val[2]);
      dest.setEntityLinearVelocity(vel);
   }

   /// support the dead reckoning data
   {
      ///\todo adjust more settings
      ///\todo verify the algorithm assignments
      DIS::DeadReckoningParameter drparams;
      if(const dtGame::MessageParameter* mp = source.GetUpdateParameter(dtDIS::EnginePropertyName::DEAD_RECKONING_ALGORITHM) )
      {
         const dtDAL::NamedEnumParameter* nep = static_cast< const dtDAL::NamedEnumParameter* >( mp ) ;
         const std::string& val = nep->GetValue();

         unsigned char algo( 0 );
         if( val == dtGame::DeadReckoningAlgorithm::STATIC.GetName() )
         {
            algo = 1;
         }
         else if( val == dtGame::DeadReckoningAlgorithm::VELOCITY_ONLY.GetName() )
         {
            algo = 2;
         }
         else if( val == dtGame::DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION.GetName() )
         {
            algo = 3;
         }
         drparams.setDeadReckoningAlgorithm( algo );
      }

      dest.setDeadReckoningParameters( drparams );
   }

   // --- attempt to support the non-engine-core properties. --- //

   // this was already stored locally by the calling code,
   // but could have come from a group message parameter
   // if such a property existed, which it probably does not.
   if (const dtGame::MessageParameter* mp = source.GetUpdateParameter(dtDIS::EntityPropertyName::ENTITYID))
   {
      const dtGame::IntMessageParameter* imp = static_cast<const dtGame::IntMessageParameter*>(mp);
      unsigned short entityID = imp->GetValue();

      DIS::EntityID newID;
      newID.setApplication(eid.getApplication());
      newID.setSite(eid.getSite());
      newID.setEntity(entityID);

      dest.setEntityID(newID);
   }


   //deprecated?  Probably handled better via the SMOKE/FLAMES parameters
   //if(const dtGame::MessageParameter* mp = source.GetUpdateParameter(dtDIS::EntityPropertyName::APPEARANCE) )
   //{
   //   const dtGame::IntMessageParameter* imp = static_cast<const dtGame::IntMessageParameter*>( mp );
   //   int appearance = imp->GetValue();
   //   dest.setEntityAppearance(appearance);
   //}

   //Force ID from ActorUpdateMesage to EntityStatePDU
   if (const dtGame::MessageParameter* mp = source.GetUpdateParameter(dtDIS::EnginePropertyName::FORCE_ID))
   {
      // DIS ForceID actor property
      const dtGame::IntMessageParameter* enumMp = static_cast<const dtGame::IntMessageParameter*>(mp);
      const int val = enumMp->GetValue();
      dest.setForceId(val);
   }
}


///\todo use dtUtil::Coordinates::ConvertToLocalRotation for ENTITY_ORIENTATION.
///\todo implement dtHLAGM::RPRParameterTranslator::MapFromVelocityVectorToMessageParam for ENTITY_LINEARY_VELOCITY.
///\todo implement dtHLAGM::RPRParameterTranslator::MapFromAngularVelocityVectorToMessageParam for ANGULAR_VELOCITY.
void PartialApplicator::operator ()(const DIS::EntityStatePdu& source,
                                    dtGame::ActorUpdateMessage& dest,
                                    dtDIS::SharedState* config) 
{
   dtDAL::NamedParameter* mp ;

   // position //
   const DIS::Vector3Double& pos = source.getEntityLocation() ;
   osg::Vec3 v3(pos.getX(), pos.getY(), pos.getZ());

   if (config != NULL)
   {
      v3 = config->GetCoordinateConverter().ConvertToLocalTranslation(v3);
   }

   // dtDIS Actor Property Name 
   if ((mp = dest.AddUpdateParameter(dtDIS::EnginePropertyName::LAST_KNOWN_LOCATION, dtDAL::DataType::VEC3)))
   {
      dtGame::Vec3MessageParameter* v3mp = static_cast<dtGame::Vec3MessageParameter*>(mp);
      v3mp->SetValue(v3);
   }

   // euler angles //
   osg::Vec3 xyzRot;

   const DIS::Orientation& orie = source.getEntityOrientation();
   xyzRot[0] = osg::RadiansToDegrees(orie.getPsi());
   xyzRot[1] = osg::RadiansToDegrees(orie.getTheta());
   xyzRot[2] = osg::RadiansToDegrees(orie.getPhi());

   if (config != NULL)
   {
      xyzRot = config->GetCoordinateConverter().ConvertToLocalRotation(xyzRot);
   }

   // dtDIS Actor Property Name
   if ((mp = dest.AddUpdateParameter(dtDIS::EnginePropertyName::LAST_KNOWN_ORIENTATION, dtDAL::DataType::VEC3)))
   {
      dtDAL::NamedVec3Parameter* v3mp = static_cast< dtDAL::NamedVec3Parameter*>(mp);
      v3mp->SetValue(xyzRot);
   }

   // velocity //

   // dtDIS Actor Property Name
   if ((mp = dest.AddUpdateParameter(dtDIS::EnginePropertyName::ENTITY_LINEARY_VELOCITY, dtDAL::DataType::VEC3 )))
   {
      //TODO convert to local coordinate system?
      const DIS::Vector3Float& lv = source.getEntityLinearVelocity();
      osg::Vec3 vel(lv.getX(), lv.getY(), lv.getZ());
      dtDAL::NamedVec3Parameter* v3mp = static_cast<dtDAL::NamedVec3Parameter*>(mp);
      v3mp->SetValue(vel);
   }

   //Smoke plume
   if (ValueMap::CanHaveSmokePlume(source.getEntityType()))
   {
      if ((mp = dest.AddUpdateParameter(dtDIS::EnginePropertyName::SMOKE_PLUME_PRESENT, dtDAL::DataType::BOOLEAN)))
      {
         dtDAL::NamedBooleanParameter* boolAP = static_cast<dtDAL::NamedBooleanParameter*>(mp);

         if (dtUtil::Bits::Has(source.getEntityAppearance(), SMOKING_BIT) ||
             dtUtil::Bits::Has(source.getEntityAppearance(), SMOKING2_BIT))
         {
            boolAP->SetValue(true);
         }
         else
         {
            boolAP->SetValue(false);
         }         
      }
   }

   //flames present
   if (ValueMap::CanHaveFlames(source.getEntityType()))
   {
      if ((mp = dest.AddUpdateParameter(dtDIS::EnginePropertyName::FLAMES_PRESENT, dtDAL::DataType::BOOLEAN)))
      {
         dtDAL::NamedBooleanParameter* boolAP = static_cast<dtDAL::NamedBooleanParameter*>(mp);

         if (dtUtil::Bits::Has(source.getEntityAppearance(), FLAMING_BIT))
         {
            boolAP->SetValue(true);
         }
         else
         {
            boolAP->SetValue(false);
         }         
      }
   }

#if 0
   UpdateAcceleration( vel ) ;

   if( mp = dest.AddUpdateParameter( dtDIS::HLABaseEntityPropertyName::PROPERTY_ACCELERATION_VECTOR , dtDAL::DataType::VEC3 ) )
   {
      dtDAL::NamedVec3Parameter* v3mp = static_cast< dtDAL::NamedVec3Parameter* > ( mp ) ;
      v3mp->SetValue( mAcceleration ) ;
   }
#endif

   //TODO: add angular velocity vector

   // articulation support
   unsigned char art_param_count=source.getArticulationParameters().size();
   if( art_param_count > 0 )
   {
      mp = dest.AddUpdateParameter( dtDIS::EnginePropertyName::ARTICULATION, dtDAL::DataType::GROUP );
      if( mp != NULL )
      {
         dtDAL::NamedGroupParameter* articulation_group = static_cast<dtDAL::NamedGroupParameter*>( mp );

         // respond to each articulation parameter
         const std::vector<DIS::ArticulationParameter>& params = source.getArticulationParameters();
         if( params.size() != art_param_count )
         {  // some debug help, error checking
            LOG_DEBUG("Parameter count does not equal the number of parameters received.")
         }
         for(char art_param_index=0; art_param_index<art_param_count; ++art_param_index)
         {
            char designator = params[art_param_index].getParameterTypeDesignator();
            switch( designator )
            {
            case dtDIS::Articulation::ARTICULATED_PART:
               {
                  AddArticulationMessageParameters( params[art_param_index], articulation_group, art_param_index );
               } break;

            case dtDIS::Articulation::ATTACHED_PART:
               {
                  LOG_INFO("No articulation support for attached parts.")
               } break;

            default:
               {
                  LOG_ERROR("Unknown Articulation Parameter Type Designator with value: " + dtUtil::ToString(designator) )
               } break;
            }  // end switch
         } // end for
      } // end if( mp
   }  // end if( art_param_count
}

#if 0
void PartialApplicator::UpdateAcceleration( osg::Vec3& currentVelocity )
{
   // TODO: determine mUpdateTimeFrame between each EntityState PDU received

 //  mTimeRec = ??

   mAcceleration = ( currentVelocity - mLastVelocity ) / /* mTimeRec */ 0.01f ;
   mLastVelocity = currentVelocity ;
}
#endif

void PartialApplicator::AddArticulationMessageParameters(const DIS::ArticulationParameter& source,
                                                         dtDAL::NamedGroupParameter* topgroup,
                                                         unsigned int param_index) const
{
   int parametertype = source.getParameterType();
   int typemetric = DIS::Convert::GetArticulationTypeMetric( parametertype );
   int typeclass = DIS::Convert::GetArticulationTypeClass( parametertype );

   // in short, the name is arbitrary, but the 'r' is important.
   // this really incredible naming convention was not invented by the original author.
   // however, the original author supported it in order to gain the support of the actor, dvte::IG::Entity.
   const char second_letter_in_name = 'r';
   char name_array[3];
   name_array[0] = param_index + 1;  // never let the first letter value be '0'
   name_array[1] = second_letter_in_name;
   name_array[2] = '\0';
   dtDAL::NamedGroupParameter* datagroup = new dtDAL::NamedGroupParameter( name_array );
   topgroup->AddParameter( *datagroup );

   AddPartParameter(typeclass, datagroup);
   AddMotionParameter(typemetric, source.getParameterValue(), datagroup);
}

void PartialApplicator::AddPartParameter(unsigned int partclass, dtDAL::NamedGroupParameter* parent) const
{
   // find the node to modify
   std::string nodename;
   if( ValueMap::GetArticulationNodeName(partclass, nodename) )
   {
      AddStringParam( dtDIS::EnginePropertyName::DOF_NODE_NAME, nodename, parent);
   }
   else
   {
      LOG_DEBUG("Unsupported Articulation type class of value: " + dtUtil::ToString(partclass))
   }
}

void PartialApplicator::AddMotionParameter(unsigned int motionclass, double motionvalue, dtDAL::NamedGroupParameter* parent) const
{
   // find the property to modify
   std::string propertyname;
   if( ValueMap::GetArticulationMotionPropertyName(motionclass, propertyname) )
   {
      AddFloatParam( propertyname, motionvalue, parent);
   }
   else
   {
      LOG_DEBUG("Unsupported Articulation type metric of value: " + dtUtil::ToString(motionclass))
   }
}

void PartialApplicator::AddFloatParam(const std::string& name, float value, dtDAL::NamedGroupParameter* parent) const
{
   dtDAL::NamedFloatParameter* param = new dtDAL::NamedFloatParameter( name );
   param->SetValue( value );
   parent->AddParameter( *param );
}

void PartialApplicator::AddStringParam(const std::string& name, const std::string& value, dtDAL::NamedGroupParameter* parent) const
{
   dtDAL::NamedStringParameter* param = new dtDAL::NamedStringParameter( name );
   param->SetValue( value );
   parent->AddParameter( *param );
}


