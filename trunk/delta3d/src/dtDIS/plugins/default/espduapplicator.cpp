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

#include <dtDAL/actorproxy.h>
#include <dtDAL/namedparameter.h>
#include <dtGame/deadreckoningcomponent.h>

#include <sstream>

using namespace dtDIS::details;

void FullApplicator::operator ()(const DIS::EntityStatePdu& source,
                                 dtGame::ActorUpdateMessage& dest,
                                 const dtDIS::SharedState* config) const
{
   PartialApplicator partial;
   partial( source, dest );

   dtDAL::NamedParameter* mp;

   mp = dest.AddUpdateParameter( dtDIS::EnginePropertyName::RESOURCE_DAMAGE_OFF , dtDAL::DataType::STATIC_MESH );
   if( mp != NULL )
   {
      if (config != NULL)
      {
         const ResourceMapConfig& resources = config->GetResourceMap();
         const dtDAL::ResourceDescriptor* rdPtr;
         bool found = resources.GetMappedResource( source.getEntityType(), rdPtr ) ;
         if( found )
         {
            dtDAL::NamedResourceParameter* nrp = static_cast<dtDAL::NamedResourceParameter*>( mp );
            nrp->SetValue( rdPtr );
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

   // ground clamping property
   {
      bool doclamp(false);
      bool issupported = ValueMap::GetRequiresGroundClamping( source.getEntityType(),doclamp );
      if( issupported )
      {
         dtDAL::NamedBooleanParameter* nbpptr = new dtDAL::NamedBooleanParameter(dtDIS::EnginePropertyName::GROUND_CLAMP);
         ///\todo BMH actor likes "do flying",
         /// so use the opposite until delta3d offers an actor that uses this property.
         nbpptr->SetValue( !doclamp );
         dest.AddUpdateParameter( *nbpptr );
      }
      else
      {
         std::ostringstream strm;
         strm << "Can not determine if ground clamping is needed for Entity:" << std::endl
              << source.getEntityID() << std::endl
              << "of type:" << std::endl
              << source.getEntityType();
         LOG_ERROR( strm.str() )
         ///\todo should not have added the parameter, so remove it here, or change the code above.
      }
   }
}

void FullApplicator::operator ()(const dtGame::ActorUpdateMessage& source,
                                 const DIS::EntityID& eid,
                                 DIS::EntityStatePdu& dest,
                                 const dtDIS::SharedState* config) const
{
   // --- support the engine-core properties. --- //

   if ( const dtGame::MessageParameter* mp = source.GetUpdateParameter( dtDIS::EnginePropertyName::TRANSLATION ) )
   {
      // DIS EntityState actor property
      const dtGame::Vec3MessageParameter* v3mp = static_cast<const dtGame::Vec3MessageParameter*>( mp ) ;
      const osg::Vec3& val = v3mp->GetValue() ;
      DIS::Vector3Double loc ;
      loc.setX( val[0] ) ;
      loc.setY( val[1] ) ;
      loc.setZ( val[2] ) ;
      dest.setEntityLocation( loc ) ;
   }
   else if ( const dtGame::MessageParameter* mp = source.GetUpdateParameter( dtDIS::HLABaseEntityPropertyName::PROPERTY_LAST_KNOWN_TRANSLATION ) )
   {
      // HLA-DVTE actor property
      const dtGame::Vec3MessageParameter* v3mp = static_cast<const dtGame::Vec3MessageParameter*>( mp ) ;
      const osg::Vec3& val = v3mp->GetValue() ;
      DIS::Vector3Double loc ;
      loc.setX( val[0] ) ;
      loc.setY( val[1] ) ;
      loc.setZ( val[2] ) ;
      dest.setEntityLocation( loc ) ;
   }

   if ( const dtGame::MessageParameter* mp = source.GetUpdateParameter( dtDIS::EnginePropertyName::ROTATION ) )
   {
      // DIS EntityState actor property
      const dtGame::Vec3MessageParameter* v3mp = static_cast<const dtGame::Vec3MessageParameter*>( mp ) ;
      const osg::Vec3& val = v3mp->GetValue() ;
      DIS::Orientation orie ;
      orie.setPhi( val[0] ) ;
      orie.setTheta( val[1] ) ;
      orie.setPsi( val[2] ) ;
      dest.setEntityOrientation( orie ) ;
   }
   else if (  const dtGame::MessageParameter* mp = source.GetUpdateParameter( dtDIS::HLABaseEntityPropertyName::PROPERTY_LAST_KNOWN_ROTATION ) )
   {
      // HLA-DVTE actor property
      const dtGame::Vec3MessageParameter* v3mp = static_cast<const dtGame::Vec3MessageParameter*>( mp ) ;
      const osg::Vec3& val = v3mp->GetValue() ;
      DIS::Orientation orie ;
      orie.setPhi( val[0] ) ;
      orie.setTheta( val[1] ) ;
      orie.setPsi( val[2] ) ;
      dest.setEntityOrientation( orie ) ;
   }

   if ( const dtGame::MessageParameter* mp = source.GetUpdateParameter( dtDIS::EnginePropertyName::VELOCITY ) )
   {
      // DIS EntityState actor property
      const dtGame::Vec3MessageParameter* v3mp = static_cast<const dtGame::Vec3MessageParameter*>( mp );
      const osg::Vec3& val = v3mp->GetValue() ;
      DIS::Vector3Float vel ;
      vel.setX( val[0] ) ;
      vel.setY( val[1] ) ;
      vel.setZ( val[2] ) ;
      dest.setEntityLinearVelocity( vel ) ;
   }
   else if ( const dtGame::MessageParameter* mp = source.GetUpdateParameter( dtDIS::HLABaseEntityPropertyName::PROPERTY_VELOCITY_VECTOR ) )
   {
      // HLA-DVTE actor property
      const dtGame::Vec3MessageParameter* v3mp = static_cast<const dtGame::Vec3MessageParameter*>( mp ) ;
      const osg::Vec3& val = v3mp->GetValue() ;
      DIS::Vector3Float vel ;
      vel.setX( val[0] ) ;
      vel.setY( val[1] ) ;
      vel.setZ( val[2] ) ;
      dest.setEntityLinearVelocity( vel ) ;
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
   dest.setEntityID( eid );

   if(const dtGame::MessageParameter* mp = source.GetUpdateParameter(dtDIS::EntityPropertyName::APPEARANCE) )
   {
      const dtGame::IntMessageParameter* imp = static_cast<const dtGame::IntMessageParameter*>( mp );
      int appearance = imp->GetValue();
      dest.setEntityAppearance( appearance );
   }
}


///\todo use dtUtil::Coordinates::ConvertToLocalTranslation  for TRANSLATION.
///\todo use dtUtil::Coordinates::ConvertToLocalRotation for ROTATION.
///\todo implement dtHLAGM::RPRParameterTranslator::MapFromVelocityVectorToMessageParam for VELOCITY.
///\todo implement dtHLAGM::RPRParameterTranslator::MapFromAngularVelocityVectorToMessageParam for ANGULAR_VELOCITY.
void PartialApplicator::operator ()( const DIS::EntityStatePdu& source , dtGame::ActorUpdateMessage& dest ) 
{

   dtDAL::NamedParameter* mp ;

   // position //
   const DIS::Vector3Double& pos = source.getEntityLocation() ;
   osg::Vec3 v3( pos.getX() , pos.getY() , pos.getZ() ) ;

   // dtDIS Actor Property Name 
   if ( mp = dest.AddUpdateParameter( dtDIS::EnginePropertyName::TRANSLATION , dtDAL::DataType::VEC3 ) )
   {
      dtGame::Vec3MessageParameter* v3mp = static_cast< dtGame::Vec3MessageParameter* > ( mp ) ;
      v3mp->SetValue( v3 ) ;
   }

   if (  mp = dest.AddUpdateParameter( dtDIS::EnginePropertyName::LAST_KNOWN_TRANSLATION , dtDAL::DataType::VEC3 ) )
   {
      dtGame::Vec3MessageParameter* v3mp = static_cast< dtGame::Vec3MessageParameter* > ( mp ) ;
      v3mp->SetValue( v3 ) ;
   }

   // HLA-DVTE Actor Property Name
   if ( mp = dest.AddUpdateParameter( dtDIS::HLABaseEntityPropertyName::PROPERTY_LAST_KNOWN_TRANSLATION , dtDAL::DataType::VEC3 ) )
   {
      dtGame::Vec3MessageParameter* v3mp = static_cast< dtGame::Vec3MessageParameter* > ( mp );
      v3mp->SetValue( v3 );
   }

   // euler angles //
   const DIS::Orientation& orie = source.getEntityOrientation() ;
   osg::Vec3 hpr( orie.getPhi() , orie.getTheta() , orie.getPsi() ) ;

   // dtDIS Actor Property Name
   if ( mp = dest.AddUpdateParameter( dtDIS::EnginePropertyName::ROTATION , dtDAL::DataType::VEC3 ) )
   {
      dtDAL::NamedVec3Parameter* v3mp = static_cast< dtDAL::NamedVec3Parameter* > ( mp ) ;
      v3mp->SetValue( hpr ) ;
   }

   if ( mp = dest.AddUpdateParameter( dtDIS::EnginePropertyName::LAST_KNOWN_ROTATION , dtDAL::DataType::VEC3 ) )
   {
      dtDAL::NamedVec3Parameter* v3mp = static_cast< dtDAL::NamedVec3Parameter* > ( mp ) ;
      v3mp->SetValue( hpr ) ;
   }

   // HLA-DVTE Actor Property Name
   if ( mp = dest.AddUpdateParameter( dtDIS::HLABaseEntityPropertyName::PROPERTY_LAST_KNOWN_ROTATION , dtDAL::DataType::VEC3 ) )
   {
      dtDAL::NamedVec3Parameter* v3mp = static_cast< dtDAL::NamedVec3Parameter* > ( mp ) ;
      v3mp->SetValue( hpr ) ;
   }

   // velocity //
   const DIS::Vector3Float& lv = source.getEntityLinearVelocity() ;
   osg::Vec3 vel( lv.getX() , lv.getY() , lv.getZ() ) ;

   // dtDIS Actor Property Name
   if ( mp = dest.AddUpdateParameter( dtDIS::EnginePropertyName::VELOCITY , dtDAL::DataType::VEC3 ) )
   {
      dtDAL::NamedVec3Parameter* v3mp = static_cast< dtDAL::NamedVec3Parameter* > ( mp ) ;
      v3mp->SetValue( vel ) ;
   }

   // HLA-DVTE Actor Property Name
   if( mp = dest.AddUpdateParameter( dtDIS::HLABaseEntityPropertyName::PROPERTY_VELOCITY_VECTOR , dtDAL::DataType::VEC3 ) )
   {
      dtDAL::NamedVec3Parameter* v3mp = static_cast< dtDAL::NamedVec3Parameter* > ( mp ) ;
      v3mp->SetValue( vel ) ;
   }

   // acceleration //
   // HLA-DVTE Actor Property Name

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
   unsigned char art_param_count=source.getArticulationParameterCount();
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
