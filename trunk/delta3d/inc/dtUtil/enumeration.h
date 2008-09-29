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
 * Matthew W. Campbell
 */

#ifndef DELTA_ENUMERATION
#define DELTA_ENUMERATION

#include <string>
#include <vector>
#include <ostream>
#include <dtUtil/export.h>
#include <osg/Referenced>  // for base class

//Disable visual C++ compiler warnings that seem to indicate the compiler is
//getting confused when compiling an enumeration.
#if _MSC_VER
#  pragma warning(push)
#  pragma warning(disable:4251)
#endif

namespace dtUtil
{

   /**
    * This class represents a type-safe enumeration pattern.  It allows one to
    * also enumerate an enumeration thus looking up values in a list fashion.
    */
   class DT_UTIL_EXPORT Enumeration
   {
   public:
      /**
       * @return the string representation of this enumeration.
       */
      const std::string& GetName() const
      {
         return this->mName;
      }

      /**
       * Equality test for an enumeration.  Since enumeration objects are static,
       * and only references to enumerations may be stored by the user, it is
       * safe and efficient to compare enumeration objects based on their memory
       * address.
       */
      bool operator==(const Enumeration& rhs) const
      {
         return this == &rhs;
      }

      /**
       * Inequality test for an enumeration.
       * @see operator==
       */
      bool operator!=(const Enumeration& rhs) const
      {
         return this != &rhs;
      }

      /**
       * Overloaded string compare operator for the enumeration.  This will compare
       * the string to this enumerations getName() value.
       * @param rhs
       * @return True if they are equal.
       * @note
       *  Uses the STL string compare method implying that the rules for string
       *  equality are the same as they are for the STL string compare method.
       */
      bool operator==(const std::string& rhs) const
      {
         return this->mName == rhs;
      }

      ///Overloaded inequality test for this enumeration's string value.
      bool operator!=(const std::string& rhs) const
      {
         return this->mName != rhs;
      }

      ///Overloaded less than test for this enumeration's string value.
      bool operator<(const std::string& rhs) const
      {
         return this->mName < rhs;
      }

      ///Overloaded greater than test for this enumeration's string value.
      bool operator>(const std::string& rhs) const
      {
         return this->mName > rhs;
      }

      /**
       * Overloaded less than operator.  This checks the memory addresses of the two
       * enumerations.
       * @param rhs The second enumeration to compare to this one.
       * @return True if this enumeration is less than rhs.
       * @note
       *  This method is supported by enumerations so they may be used as keys
       *  in STL containers.  Since memory addresses are guarenteed to be unique
       *  this methods works fine.  However, it really does not make sense to use
       *  this method of comparison in other circumstances.
       */
      bool operator<(const Enumeration& rhs) const
      {
         return this < &rhs;
      }

   protected:
      ///Private virtual desctructor to get rid of compile warning
      virtual ~Enumeration() {};

      /**
       * Construct the enumeration.
       *  @note
       *      When creating a new enumeration, the constructor of derived types
       *      must call addInstance(this) in order for enumerations to be
       *      enumerated.
       */
      Enumeration(const std::string& name)
      {
         this->mName = name;
      }

   private:
      ///Private assignment operator to enforce enumeration storage by reference.
      Enumeration& operator=(const Enumeration&) { return *this; }

      ///String representation of the enumeration.
      std::string mName;
      ///Private copy constructor to enforce enumeration storage by reference.
      Enumeration(const Enumeration&) { }

      ///Helper method to print enumerations to an output stream.
      friend std::ostream &operator<<(std::ostream& os, const Enumeration& e)
      {
         os << e.GetName();
         return os;
      }
   };


/**
* Helper macros used to create the static data and methods
* needed to enumerate an enumeration.
*/
#define DECLARE_ENUM(EnumType)                          \
private:                                                \
   static std::vector<EnumType*> mInstances;           \
   static std::vector<dtUtil::Enumeration*> mGenericInstances; \
   static void AddInstance(EnumType* instance) \
   {                                                  \
      EnumType::mInstances.push_back(instance);        \
      EnumType::mGenericInstances.push_back(instance);        \
   }                                                   \
public:                                                 \
   static const std::vector<EnumType*>& EnumerateType() \
   {                                                 \
      return EnumType::mInstances;                     \
   }                                                   \
                                                    \
   static const std::vector<dtUtil::Enumeration*>& Enumerate() \
   {                                                 \
      return EnumType::mGenericInstances;                     \
   }                                                   \
   \
   static EnumType* GetValueForName(const std::string& name) \
   {                                                  \
      for(unsigned i = 0; i < mInstances.size(); i++) \
      {                                      \
         if(name == mInstances[i]->GetName()) \
         {                                   \
            return mInstances[i];              \
         }                                     \
      }                                         \
      return NULL;                              \
   }

#define IMPLEMENT_ENUM(EnumType)                        \
   std::vector<EnumType*> EnumType::mInstances;        \
   std::vector<dtUtil::Enumeration*> EnumType::mGenericInstances;

}

#ifdef _MSC_VER
#   pragma warning(pop)
#endif

#endif
