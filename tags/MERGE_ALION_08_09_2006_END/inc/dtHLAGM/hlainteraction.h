/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
 * @author Eddie Johnson
 */

#ifndef DELTA_HLA_INTERACTION
#define DELTA_HLA_INTERACTION

#include <osg/Referenced>
#include <string>
#include <vector>
#include "dtHLAGM/export.h"

namespace dtHLAGM
{
   class DT_HLAGM_EXPORT HLAInteraction : public osg::Referenced
   {
      public:

         /// Constructor
         HLAInteraction(const std::string &name = "DefaultInteraction", 
            std::vector<std::string>& paramVec = std::vector<std::string>()) :
         mName(name), 
         mParameters(paramVec)
         {

         }

         /// Destructor
         ~HLAInteraction()
         {

         }

         /**
          * Retrives the name of this interaction
          * @return mName The name
          */
         const std::string& GetName() const { return mName; }

         /**
          * Sets the name of this interaction
          * @param newName The new name
          */
         void SetName(const std::string &newName) { mName = newName; }

         /**
          * Retrieves the parameter vector of this interaction
          * @return mParameters The parameter vector
          */
         const std::vector<std::string>& GetName() const { return mParameters; }

         /**
          * Retrieves the parameter vector of this interaction
          * @return mParameters The parameter vector
          */
         std::vector<std::string>& GetName() { return mParameters; }

      private:
         std::string mName;
         std::vector<std::string> mParameters;
   };
}

#endif
