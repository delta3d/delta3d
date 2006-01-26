/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author Curtiss Murphy
 */
#ifndef DELTA_LOGTAG
#define DELTA_LOGTAG

#include "dtGame/export.h"
#include <dtCore/uniqueid.h>
#include <string>

namespace dtGame
{
   /**
    * @class LogTag 
    * A simple data class for basic Log Tag information. This class has 
    * a copy constructor plus ==, !=, =, <, and > operators.
    */
   class DT_GAME_EXPORT LogTag 
   {
      public:

         virtual ~LogTag() {}

         // Constructor
         LogTag() : mSimTimeStamp(0) { }

         // Copy Constructor
         LogTag(const LogTag& toCopy) 
         { 
            mName = toCopy.mName;
            mDescription = toCopy.mDescription;
            mUniqueId = toCopy.mUniqueId;
            mSimTimeStamp = toCopy.mSimTimeStamp;
         }

         /**
          * Gets the name 
          * @return The name
          */
         const std::string& GetName() const { return mName; }

         /**
          * Sets the name
          * @param The new name
          */
         void SetName(const std::string &newName) { mName = newName; }

         /**
          * Gets the description
          * @return The description
          */
         const std::string& GetDescription() const { return mDescription; }

         /**
          * Sets the description
          * @param The new description
          */
         void SetDescription(const std::string &newDescription) { mDescription = newDescription; }

         /**
          * Gets the simulation timestamp (as opposed to real time)
          * @return The simulation based time stamp (as opposed to real time)
          */
         double GetSimTimeStamp() const { return mSimTimeStamp; }

         /**
          * Sets the sim time stamp (as opposed to real time)
          * @param The new simulation time stamp (as opposed to real time)
          */
         void SetSimTimeStamp(double newSimTimeStamp) { mSimTimeStamp = newSimTimeStamp; }
         
         /**
          * Gets the unique id
          * @return The unique id
          * @see dtCore::UniqueId
          */
         const dtCore::UniqueId& GetUniqueId() const { return mUniqueId; }

         /**
          * Sets the unique id
          * @param The new unique id
          */
         void SetUniqueId(const dtCore::UniqueId &newId) { mUniqueId = newId; }

         bool operator==(const LogTag& compareTo) const
         {
            return mUniqueId == compareTo.mUniqueId;
         }

         bool operator!=(const LogTag& compareTo) const
         {
            return mUniqueId != compareTo.mUniqueId;
         }

         bool operator<(const LogTag& compareTo) const
         {
            return mUniqueId < compareTo.mUniqueId;
         }

         bool operator>(const LogTag& compareTo) const
         {
            return mUniqueId > compareTo.mUniqueId;
         }

         LogTag& operator=(const LogTag& assignFrom) 
         {
            if (&assignFrom == this)
               return *this;

            mName = assignFrom.mName;
            mDescription = assignFrom.mDescription;
            mUniqueId = assignFrom.mUniqueId;
            mSimTimeStamp = assignFrom.mSimTimeStamp;
            return *this;
         }
         
      private:
         
         std::string mName;
         std::string mDescription;
         dtCore::UniqueId mUniqueId;
         double mSimTimeStamp;
   };
}

#endif
