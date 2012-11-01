/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 MOVES Institute
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
 * Author: Eric R. Heine
 */
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/createvectoraction.h>

#include <dtCore/stringactorproperty.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   CreateVectorAction::CreateVectorAction()
      : ActionNode()
   {
      AddAuthor("Eric R. Heine");
   }

   /////////////////////////////////////////////////////////////////////////////
   CreateVectorAction::~CreateVectorAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void CreateVectorAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);
   }

   /////////////////////////////////////////////////////////////////////////////
   void CreateVectorAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::StringActorProperty* xProp = new dtCore::StringActorProperty(
         "X", "X",
         dtCore::StringActorProperty::SetFuncType(this, &CreateVectorAction::SetX),
         dtCore::StringActorProperty::GetFuncType(this, &CreateVectorAction::GetX),
         "The x value of the vector.");

      dtCore::StringActorProperty* yProp = new dtCore::StringActorProperty(
         "Y", "Y",
         dtCore::StringActorProperty::SetFuncType(this, &CreateVectorAction::SetY),
         dtCore::StringActorProperty::GetFuncType(this, &CreateVectorAction::GetY),
         "The y value of the vector.");

      dtCore::StringActorProperty* zProp = new dtCore::StringActorProperty(
         "Z", "Z",
         dtCore::StringActorProperty::SetFuncType(this, &CreateVectorAction::SetZ),
         dtCore::StringActorProperty::GetFuncType(this, &CreateVectorAction::GetZ),
         "The z value of the vector.");

      dtCore::StringActorProperty* wProp = new dtCore::StringActorProperty(
         "W", "W",
         dtCore::StringActorProperty::SetFuncType(this, &CreateVectorAction::SetW),
         dtCore::StringActorProperty::GetFuncType(this, &CreateVectorAction::GetW),
         "The w value of the vector.");

      mVectorProp = new dtCore::StringActorProperty(
         "Vector", "Vector",
         dtCore::StringActorProperty::SetFuncType(this, &CreateVectorAction::SetVector),
         dtCore::StringActorProperty::GetFuncType(this, &CreateVectorAction::GetVector),
         "The vector to create from values.");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, xProp, false, false, false));
      mValues.push_back(ValueLink(this, yProp, false, false, false));
      mValues.push_back(ValueLink(this, zProp, false, false, false));
      mValues.push_back(ValueLink(this, wProp, false, false, false));
      mValues.push_back(ValueLink(this, mVectorProp.get(), true, true, false));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool CreateVectorAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      double x = GetDouble("X");
      double y = GetDouble("Y");
      double z = GetDouble("Z");
      double w = GetDouble("W");

      int count = GetPropertyCount("Vector");
      for (int index = 0; index < count; index++)
      {
         dtCore::DataType& vecType = GetPropertyType("Vector", index);
         if (vecType == dtCore::DataType::VEC2F)
         {
            SetVec2(osg::Vec2(x, y), "Vector", index);
         }
         else if (vecType == dtCore::DataType::VEC3F)
         {
            SetVec3(osg::Vec3(x, y, z), "Vector", index);
         }
         else if (vecType == dtCore::DataType::VEC4F)
         {
            SetVec4(osg::Vec4(x, y, z, w), "Vector", index);
         }
      }

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool CreateVectorAction::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (ActionNode::CanConnectValue(link, value))
      {
         // Result checks its own type
         if (link->GetName() == "Vector")
         {
            if (value->CanBeType(dtCore::DataType::VEC2F) ||
                value->CanBeType(dtCore::DataType::VEC3F) ||
                value->CanBeType(dtCore::DataType::VEC4F))
            {
               return true;
            }
            return false;
         }
         else if (link->GetName() == "X" || link->GetName() == "Y" ||
                  link->GetName() == "Z" || link->GetName() == "W")
         {
            if (value->CanBeType(dtCore::DataType::INT)   ||
                value->CanBeType(dtCore::DataType::FLOAT) ||
                value->CanBeType(dtCore::DataType::DOUBLE))
            {
               return true;
            }
            return false;
         }
         return true;
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateVectorAction::SetVector(const std::string& value)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string CreateVectorAction::GetVector() const
   {
      return "";
   }

   //////////////////////////////////////////////////////////////////////////
   void CreateVectorAction::SetX(const std::string& value)
   {
      mX = value;
   }

   //////////////////////////////////////////////////////////////////////////
   std::string CreateVectorAction::GetX() const
   {
      return mX;
   }

   //////////////////////////////////////////////////////////////////////////
   void CreateVectorAction::SetY(const std::string& value)
   {
      mY = value;
   }

   //////////////////////////////////////////////////////////////////////////
   std::string CreateVectorAction::GetY() const
   {
      return mY;
   }

   //////////////////////////////////////////////////////////////////////////
   void CreateVectorAction::SetZ(const std::string& value)
   {
      mZ = value;
   }

   //////////////////////////////////////////////////////////////////////////
   std::string CreateVectorAction::GetZ() const
   {
      return mZ;
   }

   //////////////////////////////////////////////////////////////////////////
   void CreateVectorAction::SetW(const std::string& value)
   {
      mW = value;
   }

   //////////////////////////////////////////////////////////////////////////
   std::string CreateVectorAction::GetW() const
   {
      return mW;
   }
}

////////////////////////////////////////////////////////////////////////////////
