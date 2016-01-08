/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2015, Caper Holdings, LLC
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* Chris Rodgers
*/

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA
#include <dtCore/osgpropertycontainers.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/enumactorproperty.h>
#include <dtCore/vectoractorproperties.h>



namespace dtCore
{
   ////////////////////////////////////////////////////////////////////
   // CONSTANTS
   ////////////////////////////////////////////////////////////////////
//   const dtCore::RefPtr<dtCore::ActorType> OsgNodeActor::TYPE(new dtCore::ActorType("Node Actor", "OSG", "Actor type that represents an OSG node."));
   //const dtCore::RefPtr<dtCore::ActorType> BillboardActor::TYPE(new dtCore::ActorType("Billboard Actor", "OSG", "Actor that represents a Billboard node.", OsgNodeActor::TYPE.get()));
//   const dtCore::RefPtr<dtCore::ActorType> DOFActor::TYPE(new dtCore::ActorType("DOF Actor", "OSG", "Actor that represents a DOF node.", OsgNodeActor::TYPE.get()));
//   const dtCore::RefPtr<dtCore::ActorType> LODActor::TYPE(new dtCore::ActorType("LOD Actor", "OSG", "Actor that represents an LOD node.", OsgNodeActor::TYPE.get()));
   //const dtCore::RefPtr<dtCore::ActorType> OccluderActor::TYPE(new dtCore::ActorType("Occluder Actor", "OSG", "Actor that represents an Occluder node.", OsgNodeActor::TYPE.get()));
   //const dtCore::RefPtr<dtCore::ActorType> SwitchActor::TYPE(new dtCore::ActorType("Switch Actor", "OSG", "Actor that represents an Switch node.", OsgNodeActor::TYPE.get()));

   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   const dtCore::RefPtr<dtCore::ObjectType> DOFProperties::OSG_DOF_PC_TYPE(
      new dtCore::ObjectType("OSG DOF", "dtCore",
         "Properties for an OSG DOF transform node."));

   const dtCore::RefPtr<dtCore::ObjectType> LODProperties::OSG_LOD_PC_TYPE(
      new dtCore::ObjectType("OSG LOD", "dtCore",
      "Properties for an OSG LOD node."));

   const dtCore::RefPtr<dtCore::ObjectType> LODRangeProperties::OSG_LOD_RANGE_PC_TYPE(
      new dtCore::ObjectType("OSG LOD Range", "dtCore",
      "Properties for a single OSG LOD range."));


   ////////////////////////////////////////////////////////////////////
   // CONSTANTS
   ////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(DOFMultOrder);
   DOFMultOrder DOFMultOrder::PRH("PRH", osgSim::DOFTransform::PRH);
   DOFMultOrder DOFMultOrder::PHR("PHR", osgSim::DOFTransform::PHR);
   DOFMultOrder DOFMultOrder::HPR("HPR", osgSim::DOFTransform::HPR);
   DOFMultOrder DOFMultOrder::HRP("HRP", osgSim::DOFTransform::HRP);
   DOFMultOrder DOFMultOrder::RPH("RPH", osgSim::DOFTransform::RPH);
   DOFMultOrder DOFMultOrder::RHP("RHP", osgSim::DOFTransform::RHP);



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   static const dtUtil::RefString GROUP_OSG_NODE("OSG Node");
   const dtUtil::RefString OsgNodeProperties::PROPERTY_NAME("Name");

   OsgNodeProperties::OsgNodeProperties()
   {}

   OsgNodeProperties::~OsgNodeProperties()
   {}

   void OsgNodeProperties::SetName(const std::string& name)
   {
      SetNodeName(name);
   }

   const std::string& OsgNodeProperties::GetName() const
   {
      return GetNodeName();
   }

   void OsgNodeProperties::BuildPropertyMap()
   {
      // Modify the Name property so that it affects the contained node name.
      AddProperty(new dtCore::StringActorProperty(
         PROPERTY_NAME.Get(),
         PROPERTY_NAME.Get(),
         dtCore::StringActorProperty::SetFuncType(this, &OsgNodeProperties::SetName),
         dtCore::StringActorProperty::GetFuncType(this, &OsgNodeProperties::GetName),
         dtUtil::RefString("Name of the node."),
         GROUP_OSG_NODE));

      // TODO: Node reference property type?
   }



   ////////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   ////////////////////////////////////////////////////////////////////////////////
   static const dtUtil::RefString GROUP_OSG_DOF("OSG DOF");
   
   const dtUtil::RefString DOFProperties::PROPERTY_CURRENT_HPR("CurrentHPR");
   const dtUtil::RefString DOFProperties::PROPERTY_MIN_HPR("MinHPR");
   const dtUtil::RefString DOFProperties::PROPERTY_MAX_HPR("MaxHPR");
   const dtUtil::RefString DOFProperties::PROPERTY_INCREMENT_HPR("IncrementHPR");
   const dtUtil::RefString DOFProperties::PROPERTY_ANIMATION_ON("AnimationOn");
   const dtUtil::RefString DOFProperties::PROPERTY_MULT_ORDER("MultOrder");

   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   void DOFProperties::SetMultOrder(const DOFMultOrder& multOrder)
   {
      if (mNode.valid())
      {
         mNode->setHPRMultOrder(multOrder.GetValue());
      }
   }

   DOFMultOrder& DOFProperties::GetMultOrder() const
   {
      return GetEnumForValue(mNode->getHPRMultOrder(), DOFMultOrder::HPR);
   }

   void DOFProperties::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      using namespace dtCore;
      using namespace dtUtil;

      static const RefString GROUP(GROUP_OSG_DOF);

      osgSim::DOFTransform* dof = &GetNode();

      AddProperty(new Vec3ActorProperty(
         PROPERTY_CURRENT_HPR,
         PROPERTY_CURRENT_HPR,
         Vec3ActorProperty::SetFuncType(dof, &osgSim::DOFTransform::setCurrentHPR),
         Vec3ActorProperty::GetFuncType(dof, &osgSim::DOFTransform::getCurrentHPR),
         RefString("Initial HPR value"),
         GROUP));

      AddProperty(new Vec3ActorProperty(
         PROPERTY_MIN_HPR,
         PROPERTY_MIN_HPR,
         Vec3ActorProperty::SetFuncType(dof, &osgSim::DOFTransform::setMinHPR),
         Vec3ActorProperty::GetFuncType(dof, &osgSim::DOFTransform::getMinHPR),
         RefString("Minimum HPR value"),
         GROUP));

      AddProperty(new Vec3ActorProperty(
         PROPERTY_MAX_HPR,
         PROPERTY_MAX_HPR,
         Vec3ActorProperty::SetFuncType(dof, &osgSim::DOFTransform::setMaxHPR),
         Vec3ActorProperty::GetFuncType(dof, &osgSim::DOFTransform::getMaxHPR),
         RefString("Maximum HPR value"),
         GROUP));

      AddProperty(new Vec3ActorProperty(
         PROPERTY_INCREMENT_HPR,
         PROPERTY_INCREMENT_HPR,
         Vec3ActorProperty::SetFuncType(dof, &osgSim::DOFTransform::setIncrementHPR),
         Vec3ActorProperty::GetFuncType(dof, &osgSim::DOFTransform::getIncrementHPR),
         RefString("HPR increment size"),
         GROUP));

      AddProperty(new BooleanActorProperty(
         PROPERTY_ANIMATION_ON,
         PROPERTY_ANIMATION_ON,
         BooleanActorProperty::SetFuncType(dof, &osgSim::DOFTransform::setAnimationOn),
         BooleanActorProperty::GetFuncType(dof, &osgSim::DOFTransform::getAnimationOn),
         RefString("Enabled state of animation for the node"),
         GROUP));

      // ENUM PROPERTIES
      typedef EnumActorProperty<dtCore::DOFMultOrder> DOFEnumProperty;
      AddProperty(new DOFEnumProperty(
         PROPERTY_MULT_ORDER,
         PROPERTY_MULT_ORDER,
         DOFEnumProperty::SetFuncType(this, &DOFProperties::SetMultOrder),
         DOFEnumProperty::GetFuncType(this, &DOFProperties::GetMultOrder),
         RefString("Sets the order of multiplication for heading, pitch and roll using the Z, X and Y axis."),
         GROUP));
   }



   ////////////////////////////////////////////////////////////////////////////////
   // LOD CONSTANTS
   ////////////////////////////////////////////////////////////////////////////////
   // Center Mode
   IMPLEMENT_ENUM(LODCenterMode);
   LODCenterMode LODCenterMode::USE_BOUNDING_SPHERE_CENTER("USE_BOUNDING_SPHERE_CENTER", osg::LOD::USE_BOUNDING_SPHERE_CENTER);
   LODCenterMode LODCenterMode::USER_DEFINED_CENTER("USER_DEFINED_CENTER", osg::LOD::USER_DEFINED_CENTER);
   LODCenterMode LODCenterMode::UNION_OF_BOUNDING_SPHERE_AND_USER_DEFINED("UNION_OF_BOUNDING_SPHERE_AND_USER_DEFINED", osg::LOD::UNION_OF_BOUNDING_SPHERE_AND_USER_DEFINED);

   // Range Mode
   IMPLEMENT_ENUM(LODRangeMode);
   LODRangeMode LODRangeMode::DISTANCE_FROM_EYE_POINT("DISTANCE_FROM_EYE_POINT", osg::LOD::DISTANCE_FROM_EYE_POINT);
   LODRangeMode LODRangeMode::PIXEL_SIZE_ON_SCREEN("PIXEL_SIZE_ON_SCREEN", osg::LOD::PIXEL_SIZE_ON_SCREEN);

   // Strings
   static const dtUtil::RefString GROUP_OSG_LOD("OSG LOD");

   const dtUtil::RefString LODProperties::PROPERTY_CENTER_MODE("CenterMode");
   const dtUtil::RefString LODProperties::PROPERTY_RANGE_MODE("RangeMode");



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   LODProperties::LODProperties(osg::LOD* lod)
      : BaseClass(lod)
   {}

   void LODProperties::SetCenterMode(const dtCore::LODCenterMode& mode)
   {
      mNode->setCenterMode(mode.GetValue());
   }

   dtCore::LODCenterMode& LODProperties::GetCenterMode() const
   {
      return GetEnumForValue(mNode->getCenterMode(), LODCenterMode::USE_BOUNDING_SPHERE_CENTER);
   }

   void LODProperties::SetRangeMode(const dtCore::LODRangeMode& mode)
   {
      mNode->setRangeMode(mode.GetValue());
   }

   dtCore::LODRangeMode& LODProperties::GetRangeMode() const
   {
      return GetEnumForValue(mNode->getRangeMode(), LODRangeMode::DISTANCE_FROM_EYE_POINT);
   }

   void LODProperties::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      using namespace dtCore;
      using namespace dtUtil;

      static const RefString GROUP(GROUP_OSG_LOD);

      osg::LOD* lod = &GetNode();

      // TODO:

      // ENUM PROPERTIES
      typedef EnumActorProperty<dtCore::LODCenterMode> LODCenterModeEnumProperty;
      AddProperty(new LODCenterModeEnumProperty(
         PROPERTY_CENTER_MODE,
         PROPERTY_CENTER_MODE,
         LODCenterModeEnumProperty::SetFuncType(this, &LODProperties::SetCenterMode),
         LODCenterModeEnumProperty::GetFuncType(this, &LODProperties::GetCenterMode),
         dtUtil::RefString("Determines how the center of object should be determined when computing which child is active."),
         GROUP));

      typedef EnumActorProperty<dtCore::LODRangeMode> LODRangeModeEnumProperty;
      AddProperty(new LODRangeModeEnumProperty(
         PROPERTY_RANGE_MODE,
         PROPERTY_RANGE_MODE,
         LODRangeModeEnumProperty::SetFuncType(this, &LODProperties::SetRangeMode),
         LODRangeModeEnumProperty::GetFuncType(this, &LODProperties::GetRangeMode),
         dtUtil::RefString("Determines how the range values should be interpreted when computing which child is active."),
         GROUP));
   }



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   OsgNodePropertiesBuilder::OsgNodePropertiesBuilder()
   {}

   OsgNodePropertiesBuilder::~OsgNodePropertiesBuilder()
   {}

   dtCore::RefPtr<dtCore::OsgNodeProperties> OsgNodePropertiesBuilder::CreatePropertiesForNode(osg::Node& node)
   {
      dtCore::RefPtr<dtCore::OsgNodeProperties> propContainer;

      const std::string nodeType = node.className();

      if (nodeType == "DOFTransform")
      {
         propContainer = new DOFProperties(dynamic_cast<osgSim::DOFTransform*>(&node));
      }
      else if (nodeType == "LOD")
      {
         propContainer = new LODProperties(dynamic_cast<osg::LOD*>(&node));
      }

      if (propContainer.valid())
      {
         propContainer->BuildPropertyMap();
      }

      return propContainer;
   }

   /*dtCore::RefPtr<dtCore::OsgNodeActor> OsgNodeActorBuilder::CreateActorForNode(osg::Node& node)
   {
      dtCore::RefPtr<dtCore::OsgNodeActor> actor;

      const std::string nodeType(node.className());
      dtCore::ActorType* actorType = NULL;

      if (nodeType == "DOFTransform")
      {
         actorType = DOFActor::TYPE.get();
      }
      else if (nodeType == "LOD")
      {
         actorType = LODActor::TYPE.get();
      }

      if (actorType != NULL)
      {
         // HACK: actor = dynamic_cast<dtCore::OsgNodeActor*>(dtCore::LibraryManager::GetInstance().CreateActor(*actorType));
      }

      return actor;
   }*/

}


