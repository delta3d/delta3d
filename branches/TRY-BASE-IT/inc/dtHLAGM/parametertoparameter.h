/*
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
 * David Guthrie
 * Olen A. Bruce
 */

#ifndef DELTA_PARAMETER_TO_PARAMETER
#define DELTA_PARAMETER_TO_PARAMETER

//
// parametertoparameter.h: Declaration of the ParamaterToParameter class.
//
///////////////////////////////////////////////////////////////////////

#include <dtHLAGM/export.h>
#include <dtHLAGM/onetoonemapping.h>
#include <dtHLAGM/onetomanymapping.h>

namespace dtHLAGM
{
   class DT_HLAGM_EXPORT ParameterToParameter : public OneToOneMapping
   {
      public:

         ParameterToParameter()
         {}

         /**
          * Constructor setting the basic values
          * @param hlaName the name of the HLA value to map to.
          * @param gameName the name of the Game value to map to.
          * @param hlaType the type of the HLA value.
          * @param gameType the datatype in the game manager.
          * @param defaultValue a string version of the default value for either side.
          * @param requiredForGame true if this field is required in the game manager.
          * @param requiredForHLA true if this field is required in the HLA FOM.
          */
         ParameterToParameter(const std::string& hlaName,
                             const std::string& gameName,
                             const AttributeType& attributeType,
                             dtCore::DataType& gameType,
                             const std::string& defaultValue,
                             bool requiredForGame,
                             bool requiredForHLA):
                             OneToOneMapping(hlaName,
                                gameName,
                                attributeType,
                                gameType,
                                defaultValue,
                                requiredForGame,
                                requiredForHLA)
         {}


         virtual ~ParameterToParameter()
         {}

         RTIParameterHandle* GetParameterHandle()
         {
            return mParameterHandle;
         }

         void SetParameterHandle(RTIParameterHandle* parameterHandle)
         {
            mParameterHandle = parameterHandle;
         }

      private:

         dtCore::RefPtr<RTIParameterHandle> mParameterHandle;
   };

   class DT_HLAGM_EXPORT ParameterToParameterList : public OneToManyMapping
   {
      public:

         ParameterToParameterList()
         {}

         /**
          * Constructor setting the basic values
          * @param hlaName the name of the HLA value to map to.
          * @param hlaType the type of the HLA value.
          * @param requiredForHLA true if this field is required in the HLA FOM.
          * @param isArray if the network data type is an array.
          */
         ParameterToParameterList(const std::string& hlaName,
                             const AttributeType& attributeType,
                             bool requiredForHLA, bool isArray = false):
                             OneToManyMapping(hlaName,
                                attributeType,
                                requiredForHLA, false, isArray)
         {}


         virtual ~ParameterToParameterList()
         {}

         RTIParameterHandle* GetParameterHandle() const
         {
            return mParameterHandle;
         }

         void SetParameterHandle(RTIParameterHandle* parameterHandle)
         {
            mParameterHandle = parameterHandle;
         }

      private:

         dtCore::RefPtr<RTIParameterHandle> mParameterHandle;
   };

}

#endif //DELTA_PARAMETER_TO_PARAMETER
