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

#ifndef DELTA_PARAMETER_TRANSLATOR
#define DELTA_PARAMETER_TRANSLATOR

#include <dtCore/base.h>
#include <dtCore/refptr.h>
#include <dtHLAGM/export.h>
#include <dtHLAGM/attributetype.h>
#include <dtGame/messageparameter.h>

#include <cstring>

namespace dtHLAGM
{
   class OneToManyMapping;
   class ObjectRuntimeMappingInfo;
   
   class ParameterTranslator : public dtCore::Base
   {
      public:
         /**
          * @parat name the string version of the attribute type.
          * @return Should return an attribute type instance that this translator would use
          *         to uniquely identify the named type.  It must return AttributeType::UNKNOWN if the name doesn't match anything.
          */
         virtual const AttributeType& GetAttributeTypeForName(const std::string& name) const = 0;

         ///@return true if this translator supports the given type.
         virtual bool TranslatesAttributeType(const AttributeType& type) const = 0;

         virtual void MapToMessageParameters(const char* buffer, 
                                             size_t size, 
                                             std::vector<dtCore::RefPtr<dtGame::MessageParameter> >& parameters, 
                                             const OneToManyMapping& mapping) const = 0;

         virtual void MapFromMessageParameters(char* buffer,
                                               size_t& maxSize,
                                               std::vector<dtCore::RefPtr<const dtGame::MessageParameter> >& parameters,
                                               const OneToManyMapping& mapping) const = 0;

         /**
          * Creates a buffer that will hold the data for the given attribute type.
          * @param buffer an output parameter that is a pointer to the buffer.
          * @param size an output parameter that is the size of the buffer.
          * @param type the AttributeType that this buffer should be created to hold.
          */
         static void AllocateBuffer(char*& buffer, size_t& size, const AttributeType& type, size_t elements = 1, size_t padding = 0)
         {
            size = type.GetEncodedLength();
            size = (size * elements) + padding;
            if (size > 0)
            {
               buffer = new char[size];
               //zero the buffer.
               memset((void*)buffer, 0, size);
            }
         }
   
         /**
          * Deallocates the buffer used to hold an HLA attribute or parameter.
          * @param buffer the buffer to deallocate. This is sent as a reference and will be set to NULL when this completes.
          */
         static void DeallocateBuffer(char*& buffer)
         {
            delete[] buffer;
            buffer = NULL;
         }
   };

}

#endif
