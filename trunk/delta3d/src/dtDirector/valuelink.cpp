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
      , mProxyOwner(NULL)
      , mLabel("NONE")
      , mVisible(true)
      , mExposed(exposed)
      , mRedirector(NULL)
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
      mProxyOwner = NULL;
      mLabel = src.mLabel;
      mVisible = src.mVisible;
      mExposed = src.mExposed;
      mRedirector = NULL;

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
   }

   ////////////////////////////////////////////////////////////////////////////////
   ValueLink& ValueLink::operator=(const ValueLink& src)
   {
      // Disconnect all values from this link.
      Disconnect();

      mOwner = src.mOwner;
      mProxyOwner = NULL;
      mLabel = src.mLabel;
      mVisible = src.mVisible;
      mExposed = src.mExposed;
      mRedirector = NULL;

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

   ////////////////////////////////////////////////////////////////////////////////
   void ValueLink::RedirectLink(ValueLink* redirector)
   {
      if (mRedirector)
      {
         mRedirector->SetProxyOwner(NULL);
      }

      mRedirector = redirector;

      if (mRedirector)
      {
         mRedirector->SetProxyOwner(GetOwner());
      }
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
         ValueNode* ownerValue = NULL;
         
         // Use the proxy if it exists.
         if (mProxyOwner.valid())
         {
            ownerValue = dynamic_cast<ValueNode*>(mProxyOwner.get());
         }
         else
         {
            ownerValue = dynamic_cast<ValueNode*>(mOwner);
         }

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
      // Redirect if needed.
      if (mRedirector)
      {
         return mRedirector->GetProperty(index, outNode);
      }

      ValueNode* valueNode = NULL;
      int count = 0;
      int subIndex = 0;
      int linkCount = (int)mLinks.size();
      for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
      {
         ValueNode* vNode = mLinks[linkIndex];
         if (vNode && vNode->GetEnabled())
         {
            int total = vNode->GetPropertyCount();

            // If our desired index is within this link, find the sub index.
            if (count + total > index)
            {
               subIndex = index - count;
               valueNode = vNode;
               break;
            }

            count += total;
         }
      }

      if (valueNode)
      {
         if (outNode) *outNode = valueNode;

         dtDAL::ActorProperty* prop = valueNode->GetProperty(subIndex);
         if (prop)
         {
            return prop;
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
      if (mRedirector) return mRedirector->GetDefaultProperty();

      return mDefaultProperty;
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueLink::SetDefaultProperty(dtDAL::ActorProperty* prop)
   {
      if (mRedirector)
      {
         mRedirector->SetDefaultProperty(prop);
         return;
      }

      mDefaultProperty = prop;
   }

   //////////////////////////////////////////////////////////////////////////
   int ValueLink::GetPropertyCount()
   {
      if (mRedirector)
      {
         return mRedirector->GetPropertyCount();
      }

      // Always return at least 1, because we always have the default.
      if (mLinks.empty())
      {
         if (GetDefaultProperty()) return 1;
         else return 0;
      }

      int count = 0;
      int linkCount = (int)mLinks.size();
      for (int index = 0; index < linkCount; index++)
      {
         ValueNode* vNode = mLinks[index];
         if (vNode && vNode->GetEnabled())
         {
            count += vNode->GetPropertyCount();
         }
      }
      return count;
   }

   //////////////////////////////////////////////////////////////////////////
   std::string ValueLink::GetName()
   {
      if (mRedirector)
      {
         return mRedirector->GetName();
      }

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
      if (mRedirector)
      {
         mRedirector->SetLabel(label);
         return;
      }

      mLabel = label;
   }

   //////////////////////////////////////////////////////////////////////////
   std::string ValueLink::GetDisplayName()
   {
      if (mRedirector)
      {
         return mRedirector->GetDisplayName();
      }

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
      if (mProxyOwner.valid())
      {
         if (!mProxyOwner->CanConnectValue(this, valueNode))
         {
            return false;
         }
      }
      else
      {
         if (!mOwner->CanConnectValue(this, valueNode))
         {
            return false;
         }
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

      if (mProxyOwner.valid()) mProxyOwner->OnLinkValueChanged(GetName());
      else mOwner->OnLinkValueChanged(GetName());

      return result;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<ValueNode*>& ValueLink::GetLinks()
   {
      if (mRedirector) return mRedirector->GetLinks();

      return mLinks;
   }
}
