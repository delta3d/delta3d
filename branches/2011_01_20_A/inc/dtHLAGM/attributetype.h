/* -*-c++-*-
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
 * David Guthrie
 */

#ifndef DELTA_ATTRIBUTE_TYPE
#define DELTA_ATTRIBUTE_TYPE

#include <dtHLAGM/export.h>
#include <dtUtil/enumeration.h>

namespace dtHLAGM 
{

   /**
    * 
    */
   class DT_HLAGM_EXPORT AttributeType : public dtUtil::Enumeration
   {
      DECLARE_ENUM(AttributeType);

      public:
      
         /**
          * An unknown type.  Used when mappings are not properly configured or a parameter translator does not recognize a type.
          */
         static const AttributeType UNKNOWN;

         ///@return The number of parameters this attribute type can translate .
         unsigned int GetSupportedParameterCount() const { return mSupportedParameterCount; }
         
         ///@return the max encoded length size for this type.  This is used when allocating buffers.
         size_t GetEncodedLength() const { return mEncodedLength; }

      protected:
      
         AttributeType(const std::string& name, unsigned int supportedParameters, size_t encodedLength): dtUtil::Enumeration(name),
            mSupportedParameterCount(supportedParameters), mEncodedLength(encodedLength)
         {
            AddInstance(this);
         }
         
         AttributeType(const AttributeType&): dtUtil::Enumeration("")
         {}

         AttributeType& operator=(const AttributeType&)
         {
            return *this;
         }
         
         virtual ~AttributeType() {}
         
         unsigned int mSupportedParameterCount;
         size_t mEncodedLength;
   };

}
#endif
