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

#include <dtDirector/outputlink.h>
#include <dtDirector/inputlink.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   OutputLink::OutputLink(Node* owner, const std::string& name)
      : mOwner(owner)
   {
      SetName(name);
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   OutputLink::~OutputLink()
   {
      // Disconnect this link from all inputs.
      Disconnect();
   }

   //////////////////////////////////////////////////////////////////////////
   void OutputLink::SetName(const std::string& name)
   {
      mName = name;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& OutputLink::GetName() const
   {
      return mName;
   }

   //////////////////////////////////////////////////////////////////////////
   void OutputLink::Activate()
   {
      // Iterate and activate all input linkes that are connected.
      for (int linkIndex = 0; linkIndex < (int)mLinks.size(); linkIndex++)
      {
         InputLink* link = mLinks[linkIndex];
         if (link)
         {
            link->Activate();
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool OutputLink::Connect(InputLink* input)
   {
      // Cannot connect a NULL node.
      if (!input)
      {
         return false;
      }

      // Make sure it isn't already connected.
      for (int outputIndex = 0; outputIndex < (int)mLinks.size(); outputIndex++)
      {
         if (mLinks[outputIndex] == input) return false;
      }

      mLinks.push_back(input);
      input->mLinks.push_back(this);
      return true;
   }

   //////////////////////////////////////////////////////////////////////////
   bool OutputLink::Disconnect(InputLink* input)
   {
      // Erase all?
      if (!input)
      {
         bool result = false;
         for (int outputIndex = 0; outputIndex < (int)mLinks.size(); outputIndex++)
         {
            input = mLinks[outputIndex];
            if (!input) continue;

            // Now remove this output from the input's list.
            for (int inputIndex = 0; inputIndex < (int)input->mLinks.size(); inputIndex++)
            {
               if (input->mLinks[inputIndex] == this)
               {
                  input->mLinks.erase(input->mLinks.begin() + inputIndex);
                  result = true;
                  break;
               }
            }
         }

         mLinks.clear();
         return result;
      }
      else
      {
         // Erase a single input.
         for (int outputIndex = 0; outputIndex < (int)mLinks.size(); outputIndex++)
         {
            if (mLinks[outputIndex] == input)
            {
               mLinks.erase(mLinks.begin() + outputIndex);

               // Now remove this output from the input's list.
               for (int inputIndex = 0; inputIndex < (int)input->mLinks.size(); outputIndex++)
               {
                  if (input->mLinks[inputIndex] == this)
                  {
                     input->mLinks.erase(input->mLinks.begin() + inputIndex);
                     break;
                  }
               }

               return true;
            }
         }
      }

      return false;
   }
}
