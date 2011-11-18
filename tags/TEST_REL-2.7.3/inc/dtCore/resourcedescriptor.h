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

#ifndef DELTA_RESOURCE_DESCRIPTOR
#define DELTA_RESOURCE_DESCRIPTOR

#include <string>
#include <functional>
#include <dtCore/export.h>
#include <iosfwd>

namespace dtCore 
{

   class DT_CORE_EXPORT ResourceDescriptor 
   {
      public:
         static const char DESCRIPTOR_SEPARATOR = ':';

         ///A NULL value type, used to represent an unassigned ResourceDescriptor
         static ResourceDescriptor NULL_RESOURCE;

         ResourceDescriptor() {}

         ResourceDescriptor(const std::string& displayName, const std::string& resourceIdentifier):
            mDisplayName(displayName), mResourceIdentifier(resourceIdentifier) {}

         ResourceDescriptor(const std::string& resourceIdentifier):
            mDisplayName(resourceIdentifier), mResourceIdentifier(resourceIdentifier) {}

         ResourceDescriptor(const ResourceDescriptor& toCopy) 
         {
            mDisplayName = toCopy.mDisplayName;
            mResourceIdentifier = toCopy.mResourceIdentifier;
         }

         ResourceDescriptor& operator=(const ResourceDescriptor& toAssign) 
            {
               mDisplayName = toAssign.mDisplayName;
               mResourceIdentifier = toAssign.mResourceIdentifier;
               return *this;
            }

         bool operator==(const ResourceDescriptor& toCompare) const 
         {
            return mResourceIdentifier == toCompare.mResourceIdentifier;
         }

         bool operator!=(const ResourceDescriptor& toCompare) const 
         {
            return mResourceIdentifier != toCompare.mResourceIdentifier;
         }

         bool operator>(const ResourceDescriptor& toCompare) const 
         {
            return mResourceIdentifier > toCompare.mResourceIdentifier;
         }

         bool operator<(const ResourceDescriptor& toCompare) const 
         {
            return mResourceIdentifier < toCompare.mResourceIdentifier;
         }

         const std::string& GetDisplayName() const { return mDisplayName; }
         const std::string& GetResourceIdentifier() const { return mResourceIdentifier; }
         const std::string  GetExtension() const;
         const std::string  GetResourceName() const;

         /**
          * Is this ResourceDescripter not set with any value?
          * @param true if not set, false if set
          */
         bool IsEmpty() const;

         /** 
          * Unset this instance.  Will erase any preivously set values.
          * @see IsEmpty()
          */
         void Clear();

      private:
         std::string mDisplayName;
         std::string mResourceIdentifier;
   };

   class IsCategorySeparator : public std::unary_function<char, bool> 
   {
      public:
         bool operator()(char c) const;
   };

   inline bool IsCategorySeparator::operator()(char c) const 
   {
      return c == ResourceDescriptor::DESCRIPTOR_SEPARATOR;
   }

   DT_CORE_EXPORT std::ostream& operator << (std::ostream& o, const dtCore::ResourceDescriptor& rd);

   DT_CORE_EXPORT std::istream& operator >> (std::istream& i, dtCore::ResourceDescriptor& rd);

}

#endif // DELTA_RESOURCE_DESCRIPTOR
