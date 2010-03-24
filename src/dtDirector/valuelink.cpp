/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeff P. Houde
 */

#include <sstream>
#include <algorithm>

#include <dtDirector/director.h>
#include <dtDirector/valuelink.h>
#include <dtDirector/valuenode.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/resourceactorproperty.h>
#include <dtDAL/resourcedescriptor.h>

#include <dtUtil/log.h>


namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   ValueLink::ValueLink(Node* owner, dtDAL::ActorProperty* prop, bool isOut, bool allowMultiple, bool typeCheck, bool exposed)
      : mOwner(owner)
      , mLabel("NONE")
      , mVisible(true)
      , mExposed(exposed)
      , mDefaultProperty(prop)
      , mIsOut(isOut)
      , mAllowMultiple(allowMultiple)
      , mTypeCheck(typeCheck)
      , mGettingType(false)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ValueLink::~ValueLink()
   {
      // Disconnect all values from this link.
      Disconnect();
   }

   ////////////////////////////////////////////////////////////////////////////////
   ValueLink::ValueLink(const ValueLink& src)
   {
      mOwner = src.mOwner;
      mLabel = src.mLabel;
      mVisible = src.mVisible;
      mExposed = src.mExposed;

      mDefaultProperty = src.mDefaultProperty;
      mIsOut = src.mIsOut;
      mAllowMultiple = src.mAllowMultiple;
      mTypeCheck = src.mTypeCheck;
      mGettingType = src.mGettingType;

      *this = src;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ValueLink& ValueLink::operator=(const ValueLink& src)
   {
      // Disconnect all values from this link.
      Disconnect();

      mOwner = src.mOwner;
      mLabel = src.mLabel;
      mVisible = src.mVisible;
      mExposed = src.mExposed;

      mDefaultProperty = src.mDefaultProperty;
      mIsOut = src.mIsOut;
      mAllowMultiple = src.mAllowMultiple;
      mTypeCheck = src.mTypeCheck;
      mGettingType = src.mGettingType;

      // Now connect this link to all output links connected to by the source.
      int count = (int)src.mLinks.size();
      for (int index = 0; index < count; index++)
      {
         Connect(src.mLinks[index]);
      }

      return *this;
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::DataType& ValueLink::GetPropertyType()
   {
      if (!mGettingType)
      {
         mGettingType = true;

         int count = (int)mLinks.size();
         for (int index = 0; index < count; index++)
         {
            ValueNode* node = mLinks[index];
            if (node && node->GetEnabled())
            {
               if (node->GetPropertyType() != dtDAL::DataType::UNKNOWN)
               {
                  dtDAL::DataType& type = node->GetPropertyType();
                  mGettingType = false;
                  return type;
               }
            }
         }

         // If the owner of this link is a value node, then copy its type.
         ValueNode* ownerValue = dynamic_cast<ValueNode*>(mOwner);
         if (ownerValue)
         {
            dtDAL::DataType& type = ownerValue->GetPropertyType();
            if (type != dtDAL::DataType::UNKNOWN)
            {
               mGettingType = false;
               return type;
            }
         }

         mGettingType = false;
      }

      if (mTypeCheck && GetDefaultProperty()) return GetDefaultProperty()->GetDataType();

      return dtDAL::DataType::UNKNOWN;
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProperty* ValueLink::GetProperty(int index, ValueNode** outNode)
   {
      if (index >= 0 && index < (int)mLinks.size())
      {
         ValueNode* node = mLinks[index];
         if (node && node->GetEnabled())
         {
            if (outNode) *outNode = node;

            dtDAL::ActorProperty* prop = node->GetProperty();
            if (prop)
            {
               return prop;
            }
         }
      }

      // If we have no overriding links, and we are looking for the first
      // instance, return the default property.
      if (index == 0)
      {
         return GetDefaultProperty();
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProperty* ValueLink::GetDefaultProperty()
   {
      return mDefaultProperty;
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueLink::SetDefaultProperty(dtDAL::ActorProperty* prop)
   {
      mDefaultProperty = prop;
   }

   //////////////////////////////////////////////////////////////////////////
   int ValueLink::GetPropertyCount()
   {
      // Always return at least 1, because we always have the default.
      if (mLinks.empty())
      {
         if (GetDefaultProperty()) return 1;
         else return 0;
      }

      return (int)mLinks.size();
   }

   //////////////////////////////////////////////////////////////////////////
   std::string ValueLink::GetName()
   {
      // Always display the default property name as the current property
      // changes based on what it is linked to.
      dtDAL::ActorProperty* prop = GetDefaultProperty();
      if (prop)
      {
         return prop->GetName().Get();
      }

      return mLabel;
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueLink::SetLabel(const std::string& label)
   {
      mLabel = label;
   }

   //////////////////////////////////////////////////////////////////////////
   std::string ValueLink::GetDisplayName()
   {
      // Always display the default property name as the current property
      // changes based on what it is linked to.
      dtDAL::ActorProperty* prop = GetDefaultProperty();
      if (prop)
      {
         std::string label = prop->GetLabel() + "<br>(" + prop->GetValueString() + ")";
         SetLabel(label);
      }

      return mLabel;
   }

   //////////////////////////////////////////////////////////////////////////
   bool ValueLink::Connect(ValueNode* valueNode)
   {
      if (!valueNode)
      {
         return false;
      }

      // Perform a type check.
      if (!mOwner->CanConnectValue(this, valueNode))
      {
         return false;
      }

      // If we are not allowing multiples, disconnect the current one first.
      if (!mAllowMultiple) Disconnect();

      // Make sure the link is not already made.
      for (int index = 0; index < (int)mLinks.size(); index++)
      {
         // Return fail because the connection is already made.
         if (mLinks[index] == valueNode) return false;
      }

      mLinks.push_back(valueNode);
      valueNode->mLinks.push_back(this);
      valueNode->OnConnectionChange();
      return true;
   }

   //////////////////////////////////////////////////////////////////////////
   bool ValueLink::Disconnect(ValueNode* valueNode)
   {
      bool result = false;

      if (!valueNode)
      {
         while (!mLinks.empty())
         {
            result |= Disconnect(mLinks[0]);
         }
      }
      else
      {
         for (int valueIndex = 0; valueIndex < (int)mLinks.size(); valueIndex++)
         {
            if (valueNode == mLinks[valueIndex])
            {
               int count = (int)valueNode->mLinks.size();
               for (int linkIndex = 0; linkIndex < count; linkIndex++)
               {
                  if (valueNode->mLinks[linkIndex] == this)
                  {
                     valueNode->mLinks.erase(valueNode->mLinks.begin() + linkIndex);
                     break;
                  }
               }
            
               mLinks.erase(mLinks.begin() + valueIndex);
               valueNode->OnConnectionChange();
 
               result = true;
               break;
            }
         }
      }

      mOwner->OnLinkValueChanged(GetName());

      return result;
   }
}
