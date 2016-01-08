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

#ifndef DELTA_OSG_PROPERTY_CONTAINERS_H
#define DELTA_OSG_PROPERTY_CONTAINERS_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA
#include <dtCore/export.h>
#include <dtCore/propertycontainer.h>
#include <dtCore/propertymacros.h>
#include <dtCore/stringactorproperty.h>
// OSG
#include <osg/LOD>
#include <osgSim/DOFTransform>



namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT OsgNodeProperties : public dtCore::PropertyContainer
   {
   public:
      typedef dtCore::PropertyContainer BaseClass;

      static const dtUtil::RefString PROPERTY_NAME;

      OsgNodeProperties();

      const std::string& GetName() const;

      void SetName(const std::string& name);

      virtual const std::string& GetNodeName() const = 0;

      virtual void SetNodeName(const std::string& name) = 0;

      virtual void BuildPropertyMap();

   protected:
      virtual ~OsgNodeProperties();
   };



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   template<typename T_Node>
   class DT_CORE_EXPORT OsgNodePropertiesT : public OsgNodeProperties
   {
   public:
      typedef OsgNodeProperties BaseClass;

      OsgNodePropertiesT(T_Node* node = nullptr)
      {
         if (node != nullptr)
         {
            SetNode(*node);
         }
         else
         {
            mNode = new T_Node;
            mNode->setName(GetNodeName());
         }
      }

      void SetNode(T_Node& node)
      {
         mNode = &node;

         SetNodeName(node.getName());
      }

      T_Node& GetNode() const
      {
         return *mNode;
      }

      /*override*/ void SetNodeName(const std::string& name)
      {
         mNode->setName(name);
      }

      /*override*/ const std::string& GetNodeName() const
      {
         return mNode->getName();
      }

   protected:
      virtual ~OsgNodePropertiesT()
      {}

      dtCore::RefPtr<T_Node> mNode;
   };



   ////////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   ////////////////////////////////////////////////////////////////////////////////
   template<typename T_Enum>
   class DT_CORE_EXPORT OSGEnum : public dtUtil::Enumeration
   {
   public:
      typedef T_Enum ValueType;
      typedef typename OSGEnum<T_Enum> OSGEnumType;

      typedef dtUtil::Enumeration BaseClass;

      T_Enum GetValue() const
      { return mValue; }

   protected:
      OSGEnum(const std::string& name, T_Enum value)
         : BaseClass(name)
         , mValue(value)
      {}

      virtual ~OSGEnum()
      {}

      T_Enum mValue;
   };

   template <typename T_OSGEnum>
   T_OSGEnum& GetEnumForValue(typename T_OSGEnum::ValueType value, T_OSGEnum& defaultEnum)
   {
      T_OSGEnum* result = &defaultEnum;

      typedef std::vector<dtUtil::Enumeration*> EnumList;
      const EnumList& enumList = T_OSGEnum::Enumerate();

      std::find_if(enumList.begin(), enumList.end(),
         [&](dtUtil::Enumeration* enumRef)->bool {
            bool found = false;

            T_OSGEnum* osgEnum = static_cast<T_OSGEnum*>(enumRef);
            if (osgEnum->GetValue() == value)
            {
               result = osgEnum;
               found = true;
            }
            return found;
      });

      return *result;
   }



   ////////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   ////////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT DOFMultOrder : public OSGEnum<osgSim::DOFTransform::MultOrder>
   {
      DECLARE_ENUM(DOFMultOrder);

   public:
      typedef OSGEnum<osgSim::DOFTransform::MultOrder> BaseClass;

      static DOFMultOrder PRH;
      static DOFMultOrder PHR;
      static DOFMultOrder HPR;
      static DOFMultOrder HRP;
      static DOFMultOrder RPH;
      static DOFMultOrder RHP;

   protected:
      DOFMultOrder(const std::string& name, osgSim::DOFTransform::MultOrder value)
         : BaseClass(name, value)
      {
         AddInstance(this);
      }

      virtual ~DOFMultOrder() {}
   };


   
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT DOFProperties : public OsgNodePropertiesT<osgSim::DOFTransform>
   {
   public:
      typedef OsgNodePropertiesT<osgSim::DOFTransform> BaseClass;

      static const dtCore::RefPtr<dtCore::ObjectType> OSG_DOF_PC_TYPE;

      static const dtUtil::RefString PROPERTY_CURRENT_HPR;
      static const dtUtil::RefString PROPERTY_MIN_HPR;
      static const dtUtil::RefString PROPERTY_MAX_HPR;
      static const dtUtil::RefString PROPERTY_INCREMENT_HPR;
      static const dtUtil::RefString PROPERTY_ANIMATION_ON;
      static const dtUtil::RefString PROPERTY_MULT_ORDER;

      DOFProperties(osgSim::DOFTransform* dof = nullptr)
         : BaseClass(dof)
      {}

      void SetMultOrder(const DOFMultOrder& multOrder);
      DOFMultOrder& GetMultOrder() const;

      /*override*/ const dtCore::ObjectType& GetObjectType() const
      { return *OSG_DOF_PC_TYPE; }

      /*override*/ void BuildPropertyMap();

   protected:
      virtual ~DOFProperties()
      {}
   };



   ////////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   ////////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT LODCenterMode : public OSGEnum<osg::LOD::CenterMode>
   {
      DECLARE_ENUM(LODCenterMode);

   public:
      typedef OSGEnum<osg::LOD::CenterMode> BaseClass;

      static LODCenterMode USE_BOUNDING_SPHERE_CENTER;
      static LODCenterMode USER_DEFINED_CENTER;
      static LODCenterMode UNION_OF_BOUNDING_SPHERE_AND_USER_DEFINED;

   protected:
      LODCenterMode(const std::string& name, osg::LOD::CenterMode value)
         : BaseClass(name, value)
      {
         AddInstance(this);
      }

      virtual ~LODCenterMode() {}
   };



   ////////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT LODRangeMode : public OSGEnum<osg::LOD::RangeMode>
   {
      DECLARE_ENUM(LODRangeMode);

   public:
      typedef OSGEnum<osg::LOD::RangeMode> BaseClass;

      static LODRangeMode DISTANCE_FROM_EYE_POINT;
      static LODRangeMode PIXEL_SIZE_ON_SCREEN;

   protected:
      LODRangeMode(const std::string& name, osg::LOD::RangeMode value)
         : BaseClass(name, value)
      {
         AddInstance(this);
      }

      virtual ~LODRangeMode() {}
   };



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT LODRangeProperties : public dtCore::PropertyContainer
   {
   public:
      typedef dtCore::PropertyContainer BaseClass;

      static const dtCore::RefPtr<dtCore::ObjectType> OSG_LOD_RANGE_PC_TYPE;

      LODRangeProperties();

      /*override*/ const dtCore::ObjectType& GetObjectType() const
      {
         return *OSG_LOD_RANGE_PC_TYPE;
      }

      void BuildPropertyMap();

   protected:
      virtual ~LODRangeProperties()
      {}
   };



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT LODProperties : public OsgNodePropertiesT<osg::LOD>
   {
   public:
      typedef OsgNodePropertiesT<osg::LOD> BaseClass;

      static const dtCore::RefPtr<dtCore::ObjectType> OSG_LOD_PC_TYPE;

      static const dtUtil::RefString PROPERTY_CENTER_MODE;
      static const dtUtil::RefString PROPERTY_RANGE_MODE;

      LODProperties(osg::LOD* lod = nullptr);

      void SetCenterMode(const dtCore::LODCenterMode& mode);
      dtCore::LODCenterMode& GetCenterMode() const;

      void SetRangeMode(const dtCore::LODRangeMode& mode);
      dtCore::LODRangeMode& GetRangeMode() const;

      /*override*/ const dtCore::ObjectType& GetObjectType() const
      { return *OSG_LOD_PC_TYPE; }

      /*override*/ void BuildPropertyMap();

   protected:
      virtual ~LODProperties()
      {}
   };

   

   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT OsgNodePropertiesBuilder : public osg::Referenced
   {
   public:
      OsgNodePropertiesBuilder();

      dtCore::RefPtr<dtCore::OsgNodeProperties> CreatePropertiesForNode(osg::Node& node);

   protected:
      virtual ~OsgNodePropertiesBuilder();
   };

}

#endif
