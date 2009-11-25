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

#include <dtDirector/inputlink.h>
#include <dtDirector/outputlink.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   InputLink::InputLink(Node* owner, const std::string& name)
      : mOwner(owner)
      , mActivated(false)
   {
      SetName(name);
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   InputLink::~InputLink()
   {
      // Disconnect this link from all outputs.
      Disconnect();
   }

   //////////////////////////////////////////////////////////////////////////
   void InputLink::SetName(const std::string& name)
   {
      mName = name;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& InputLink::GetName() const
   {
      return mName;
   }

   //////////////////////////////////////////////////////////////////////////
   void InputLink::Activate()
   {
      mActivated = true;
   }

   //////////////////////////////////////////////////////////////////////////
   void InputLink::Connect(OutputLink* output)
   {
      if (output) output->Connect(this);
   }

   //////////////////////////////////////////////////////////////////////////
   void InputLink::Disconnect(OutputLink* output)
   {
      if (!output)
      {
         while (!mLinks.empty())
         {
            mLinks[0]->Disconnect(this);
         }
      }
      else
      {
         output->Disconnect(this);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool InputLink::Test()
   {
      bool bActive = mActivated;
      mActivated = false;
      return bActive;
   }
}
