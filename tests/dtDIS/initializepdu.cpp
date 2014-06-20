#include <prefix/unittestprefix.h>

#include "initializepdu.h"

#include <DIS/EntityStatePdu.h>

using namespace dtTest;

void InitializePdu::operator ()(DIS::EntityStatePdu& pdu) const
{
   DIS::EntityType aet;
   aet.setCategory(1);
   aet.setCountry(2);
   aet.setDomain(3);
   aet.setEntityKind(4);
   aet.setExtra(5);
   aet.setSpecific(6);
   aet.setSubcategory(7);
   pdu.setAlternativeEntityType( aet );

   //typedef std::vector<DIS::ArticulationParameter> ArtParamVec;
   //ArtParamVec apv;
   //DIS::ArticulationParameter ap1;
   //ap1.setParameterTypeDesignator(8);
   //ap1.setChangeIndicator(9);
   //ap1.setPartAttachedTo(10);
   //apv.push_back(ap1);
   //DIS::ArticulationParameter ap2;
   //ap1.setParameterTypeDesignator(11);
   //ap1.setChangeIndicator(12);
   //ap1.setPartAttachedTo(13);
   //apv.push_back(ap2);
   //pdu.setArticulationParameters(apv);

   pdu.setCapabilities(14);

   ///\todo use the api for setting "other parameters"
   DIS::DeadReckoningParameter drp;
   drp.setDeadReckoningAlgorithm(15);
   DIS::Vector3Float drpla;
   drpla.setX(16);
   drpla.setY(17);
   drpla.setZ(18);
   drp.setEntityLinearAcceleration( drpla );
   DIS::Vector3Float drpav;
   drpav.setX(19);
   drpav.setY(20);
   drpav.setZ(21);
   drp.setEntityAngularVelocity( drpav );
   pdu.setDeadReckoningParameters( drp );

   pdu.setEntityAppearance(22);

   DIS::EntityID eid;
   eid.setSite(23);
   eid.setApplication(24);
   eid.setEntity(25);
   pdu.setEntityID( eid );

   DIS::Vector3Float elv;
   elv.setX(26);
   elv.setY(27);
   elv.setZ(28);
   pdu.setEntityLinearVelocity( elv );

   DIS::Vector3Double el;
   el.setX(29);
   el.setY(30);
   el.setZ(31);
   pdu.setEntityLocation( el );

   DIS::Orientation eo;
   eo.setPhi(34);
   eo.setTheta(33);
   eo.setPsi(32);
   pdu.setEntityOrientation( eo );

   DIS::EntityType et;
   et.setCategory(35);
   et.setCountry(36);
   et.setDomain(37);
   et.setEntityKind(38);
   et.setExtra(39);
   et.setSpecific(40);
   et.setSubcategory(41);
   pdu.setEntityType( et );

   pdu.setExerciseID(42);

   pdu.setForceId(43);

   ///\todo enter actual packet size here, or have the packet just know it, like it probably should
   /// best guess at size, '3' virtual functions.
   pdu.setLength(sizeof(DIS::EntityStatePdu)-3);

   //                   0   1   2   3   4   5   6   7   8   9   10  11
   char marking[12] = { 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55};
   DIS::Marking disMarking;
   disMarking.setCharacters(marking);
   pdu.setMarking(disMarking);

   ///\todo the pdu should probably assign this and the interface should not be public
   pdu.setPadding(56);

   ///\todo the pdu should probably assign this and the interface should not be public
   pdu.setPduType(57);

   ///\todo this interface should probably not be public too
   pdu.setProtocolFamily(58);

   ///\todo this interface should probably not be public too
   pdu.setProtocolVersion(59);

   pdu.setTimestamp(60);
}
