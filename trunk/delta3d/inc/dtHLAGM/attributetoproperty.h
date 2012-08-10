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
 */

#ifndef DELTA_ATTRIBUTE_TO_PROPERTY
#define DELTA_ATTRIBUTE_TO_PROPERTY

//
// attributetoproperty.h: Declaration of the AttributeToProperty class.
//
///////////////////////////////////////////////////////////////////////

#include <dtHLAGM/export.h>
#include <dtHLAGM/onetoonemapping.h>
#include <dtHLAGM/onetomanymapping.h>
#include <dtHLAGM/rtihandle.h>
#include <dtCore/refptr.h>

namespace dtHLAGM
{
   class DT_HLAGM_EXPORT AttributeToProperty : public OneToOneMapping
   {
      public:

         AttributeToProperty()
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
         AttributeToProperty(const std::string& hlaName,
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

         virtual ~AttributeToProperty()
         {}

         RTIAttributeHandle* GetAttributeHandle() const
         {
            return mAttributeHandle;
         }

         void SetAttributeHandle(RTIAttributeHandle* attributeHandle)
         {
            mAttributeHandle = attributeHandle;
         }

      private:

         dtCore::RefPtr<RTIAttributeHandle> mAttributeHandle;
   };

   class DT_HLAGM_EXPORT AttributeToPropertyList : public OneToManyMapping
   {
      public:

         AttributeToPropertyList()
         {}

         /**
          * Constructor setting the basic values
          * @param hlaName the name of the HLA value to map to.
          * @param hlaType the type of the HLA value.
          * @param requiredForHLA true if this field is required in the HLA FOM.
          */
         AttributeToPropertyList(const std::string& hlaName,
                             const AttributeType& attributeType,
                             bool requiredForHLA):
                             OneToManyMapping(hlaName,
                                attributeType,
                                requiredForHLA)
         {}

         virtual ~AttributeToPropertyList()
         {}

         RTIAttributeHandle* GetAttributeHandle() const
         {
            return mAttributeHandle;
         }

         void SetAttributeHandle(RTIAttributeHandle* attributeHandle)
         {
            mAttributeHandle = attributeHandle;
         }

      private:

         dtCore::RefPtr<RTIAttributeHandle> mAttributeHandle;
   };

}

#endif //DELTA_ATTRIBUTE_TO_PROPERTY

