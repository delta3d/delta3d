#include "initializeupdatemessage.h"

#include <dtGame/messageparameter.h>
#include <dtDIS/propertyname.h>
#include <dtUtil/matrixutil.h>
#include <dtGame/actorupdatemessage.h>

#include <DIS/EntityStatePdu.h>

using namespace dtTest;

void InitializeUpdateMessage::operator ()(dtGame::ActorUpdateMessage& msg) const
{
   dtGame::MessageParameter* mp;

   //DIS::EntityType aet;
   //aet.setCategory(1);
   //mp = msg.AddUpdateParameter("dis_alternative_entitytype_category", dtDAL::DataType::CHAR );
   //mp->FromString( dtUtil::ToString(aet.getCategory()) );
   //aet.setCountry(2);
   //mp = msg.AddUpdateParameter("dis_alternative_entitytype_country", dtDAL::DataType::SHORTINT );
   //mp->FromString( dtUtil::ToString(aet.getCountry()) );
   //aet.setDomain(3);
   //mp = msg.AddUpdateParameter("dis_alternative_entitytype_domain", dtDAL::DataType::CHAR );
   //mp->FromString( dtUtil::ToString(aet.getDomain()) );
   //aet.setEntityKind(4);
   //mp = msg.AddUpdateParameter("dis_alternative_entitytype_entitykind", dtDAL::DataType::CHAR );
   //mp->FromString( dtUtil::ToString(aet.getEntityKind()) );
   //aet.setExtra(5);
   //mp = msg.AddUpdateParameter("dis_alternative_entitytype_extra", dtDAL::DataType::CHAR );
   //mp->FromString( dtUtil::ToString(aet.getExtra()) );
   //aet.setSpecific(6);
   //mp = msg.AddUpdateParameter("dis_alternative_entitytype_specific", dtDAL::DataType::CHAR );
   //mp->FromString( dtUtil::ToString(aet.getSpecific()) );
   //aet.setSubcategory(7);
   //mp = msg.AddUpdateParameter("dis_alternative_entitytype_subcategory", dtDAL::DataType::CHAR );
   //mp->FromString( dtUtil::ToString(aet.getSubcategory()) );

   /////\todo use '2', and set the params on the msg below
   ////pdu.setArticulationParameterCount(2);
   //mp = msg.AddUpdateParameter("dis_articulation_parameter_count", dtDAL::DataType::CHAR );
   //mp->FromString( dtUtil::ToString(0) );

   ////typedef std::vector<DIS::ArticulationParameter> ArtParamVec;
   ////ArtParamVec apv;
   ////DIS::ArticulationParameter ap1;
   ////ap1.setParameterTypeDesignator(8);
   ////ap1.setChangeIndicator(9);
   ////ap1.setPartAttachedTo(10);
   ////apv.push_back(ap1);
   ////DIS::ArticulationParameter ap2;
   ////ap1.setParameterTypeDesignator(11);
   ////ap1.setChangeIndicator(12);
   ////ap1.setPartAttachedTo(13);
   ////apv.push_back(ap2);
   ////pdu.setArticulationParameters(apv);

   ////pdu.setCapabilities(14);
   //mp = msg.AddUpdateParameter("dis_capabilities", dtDAL::DataType::INT );
   //mp->FromString( dtUtil::ToString(14) );

   /////\todo use the api for setting "other parameters"
   //DIS::DeadReckoningParameter drp;
   //drp.setDeadReckoningAlgorithm(15);
   //mp = msg.AddUpdateParameter("dis_dead_reckoning_algorithm", dtDAL::DataType::CHAR );
   //mp->FromString( dtUtil::ToString(drp.getDeadReckoningAlgorithm()) );
   //DIS::Vector3Float drpla;
   //drpla.setX(16);
   //drpla.setY(17);
   //drpla.setZ(18);
   //drp.setEntityLinearAcceleration( drpla );
   ////mp = msg.AddUpdateParameter("dis_dead_reckoning_linear_acceleration", dtDAL::DataType::VEC3F );
   ////mp->FromString( dtUtil::ToString(drp.getEntityLinearAcceleration()) );
   //DIS::Vector3Float drpav;
   //drpav.setX(19);
   //drpav.setY(20);
   //drpav.setZ(21);
   //drp.setEntityAngularVelocity( drpav );
   ////mp = msg.AddUpdateParameter("dis_dead_reckoning_entity_angular_velocity", dtDAL::DataType::VEC3F );
   ////mp->FromString( dtUtil::ToString(drp.getEntityAngularVelocity()) );

   //pdu.setEntityAppearance(22);
   mp = msg.AddUpdateParameter(dtDIS::EntityPropertyName::APPEARANCE, dtDAL::DataType::INT );
   mp->FromString( dtUtil::ToString(22) );

   DIS::EntityID eid;
   eid.setSite(23);
   //mp = msg.AddUpdateParameter(dtDIS::PropertyName::ENTITYID_SITE, dtDAL::DataType::SHORTINT );
   //mp->FromString( dtUtil::ToString(eid.getSite()) );
   eid.setApplication(24);
   //mp = msg.AddUpdateParameter(dtDIS::PropertyName::ENTITYID_APPLICATION, dtDAL::DataType::SHORTINT );
   //mp->FromString( dtUtil::ToString(eid.getApplication()) );
   eid.setEntity(25);
   //////mp = msg.AddUpdateParameter(dtDIS::PropertyName::ENTITYID_ENTITY, dtDAL::DataType::SHORTINT );
   //////mp->FromString( dtUtil::ToString(eid.getEntity()) );

   mp = msg.AddUpdateParameter(dtDIS::EnginePropertyName::ENTITY_LINEARY_VELOCITY, dtDAL::DataType::VEC3 );
   if (mp != NULL)
   {
      dtGame::Vec3MessageParameter *v3mp = static_cast<dtGame::Vec3MessageParameter*>(mp);
      v3mp->SetValue( osg::Vec3( 26, 27, 28 ) );
   }

   mp = msg.AddUpdateParameter( dtDIS::EnginePropertyName::ENTITY_LOCATION , dtDAL::DataType::VEC3 );
   if( mp != NULL )
   {
      dtGame::Vec3MessageParameter* v3mp = static_cast<dtGame::Vec3MessageParameter*>( mp );
      v3mp->SetValue( osg::Vec3(29, 30, 31) );
   }

   mp = msg.AddUpdateParameter( dtDIS::EnginePropertyName::ENTITY_ORIENTATION , dtDAL::DataType::VEC3 );
   if( mp != NULL )
   {
      dtGame::Vec3MessageParameter* v3mp = static_cast<dtGame::Vec3MessageParameter*>( mp );
      v3mp->SetValue( osg::Vec3(34, 33, 32) );
   }

   mp = msg.AddUpdateParameter(dtDIS::EnginePropertyName::LAST_KNOWN_ORIENTATION, dtDAL::DataType::VEC3);
   if( mp != NULL )
   {
      dtGame::Vec3MessageParameter* v3mp = static_cast<dtGame::Vec3MessageParameter*>( mp );
      v3mp->SetValue( osg::Vec3(34, 33, 32) );
   }


   mp = msg.AddUpdateParameter(dtDIS::EnginePropertyName::LAST_KNOWN_LOCATION, dtDAL::DataType::VEC3);
   if( mp != NULL )
   {
      dtGame::Vec3MessageParameter* v3mp = static_cast<dtGame::Vec3MessageParameter*>( mp );
      v3mp->SetValue( osg::Vec3(29, 30, 31) );
   }

   //DIS::EntityType et;
   //et.setCategory(35);
   //mp = msg.AddUpdateParameter("dis_entitytype_category", dtDAL::DataType::CHAR );
   //mp->FromString( dtUtil::ToString(et.getCategory()) );
   //et.setCountry(36);
   //mp = msg.AddUpdateParameter("dis_entitytype_country", dtDAL::DataType::SHORTINT );
   //mp->FromString( dtUtil::ToString(et.getCountry()) );
   //et.setDomain(37);
   //mp = msg.AddUpdateParameter("dis_entitytype_domain", dtDAL::DataType::CHAR );
   //mp->FromString( dtUtil::ToString(et.getDomain()) );
   //et.setEntityKind(38);
   //mp = msg.AddUpdateParameter("dis_entitytype_entitykind", dtDAL::DataType::CHAR );
   //mp->FromString( dtUtil::ToString(et.getEntityKind()) );
   //et.setExtra(39);
   //mp = msg.AddUpdateParameter("dis_entitytype_extra", dtDAL::DataType::CHAR );
   //mp->FromString( dtUtil::ToString(et.getExtra()) );
   //et.setSpecific(40);
   //mp = msg.AddUpdateParameter("dis_entitytype_specific", dtDAL::DataType::CHAR );
   //mp->FromString( dtUtil::ToString(et.getSpecific()) );
   //et.setSubcategory(41);
   //mp = msg.AddUpdateParameter("dis_entitytype_subcategory", dtDAL::DataType::CHAR );
   //mp->FromString( dtUtil::ToString(et.getSubcategory()) );
   ////pdu.setEntityType( et );

   ////pdu.setExerciseID(42);
   //mp = msg.AddUpdateParameter("dis_exerciseid", dtDAL::DataType::UCHAR );
   //mp->FromString( dtUtil::ToString(42) );

   ////pdu.setForceId(43);
   //mp = msg.AddUpdateParameter("dis_forceid", dtDAL::DataType::UCHAR );
   //mp->FromString( dtUtil::ToString(43) );

   /////\todo enter actual packet size here, or have the packet just know it, like it probably should
   ///// best guess at size, '3' virtual functions.
   ////pdu.setLength(sizeof(DIS::EntityStatePdu)-3);
   //mp = msg.AddUpdateParameter("dis_length", dtDAL::DataType::USHORTINT );
   //mp->FromString( dtUtil::ToString(sizeof(DIS::EntityStatePdu)-3) );

   //////                   0   1   2   3   4   5   6   7   8   9   10  11
   ////char marking[12] = { 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55};
   ////pdu.setMarking(marking);

   /////\todo the pdu should probably assign this and the interface should not be public
   ////pdu.setPadding(56);
   //mp = msg.AddUpdateParameter("dis_padding", dtDAL::DataType::SHORTINT );
   //mp->FromString( dtUtil::ToString(56) );

   /////\todo the pdu should probably assign this and the interface should not be public
   ////pdu.setPduType(57);
   //mp = msg.AddUpdateParameter("dis_pdutype", dtDAL::DataType::UCHAR );
   //mp->FromString( dtUtil::ToString(57) );

   /////\todo this interface should probably not be public too
   ////pdu.setProtocolFamily(58);
   //mp = msg.AddUpdateParameter("dis_protocolfamily", dtDAL::DataType::UCHAR );
   //mp->FromString( dtUtil::ToString(58) );

   /////\todo this interface should probably not be public too
   ////pdu.setProtocolVersion(59);
   //mp = msg.AddUpdateParameter("dis_protocolversion", dtDAL::DataType::UCHAR );
   //mp->FromString( dtUtil::ToString(59) );

   ////pdu.setTimestamp(60);
   //mp = msg.AddUpdateParameter("dis_timestamp", dtDAL::DataType::UINT );
   //mp->FromString( dtUtil::ToString(60) );
}
