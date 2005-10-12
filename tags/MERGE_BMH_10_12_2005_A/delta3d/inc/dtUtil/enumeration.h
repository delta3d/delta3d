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
* @author Matthew W. Campbell
*/
#ifndef __Enumeration__h
#define __Enumeration__h

#include <string>
#include <vector>
#include "dtUtil/export.h"
#include "osg/Referenced"  // for base class

//Disable visual C++ compiler warnings that seem to indicate the compiler is
//getting confused when compiling an enumeration.
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#pragma warning(disable:4251)
#endif

namespace dtUtil {

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
      const std::string &GetName() const {
         return this->name;
      }

      /**
      * Equality test for an enumeration.  Since enumeration objects are static,
      * and only references to enumerations may be stored by the user, it is
      * safe and efficient to compare enumeration objects based on their memory
      * address.
      */
      bool operator==(const Enumeration &rhs) const {
		  return this->name == rhs.name;
      }

      /**
      * Inequality test for an enumeration.
      * @see operator==
      */
      bool operator!=(const Enumeration &rhs) const {
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
      bool operator==(const std::string &rhs) const {
         return this->name == rhs;
      }

      ///Overloaded inequality test for this enumeration's string value.
      bool operator!=(const std::string &rhs) const {
         return this->name != rhs;
      }

      ///Overloaded less than test for this enumeration's string value.
      bool operator<(const std::string &rhs) const {
         return this->name < rhs;
      }

      ///Overloaded greater than test for this enumeration's string value.
      bool operator>(const std::string &rhs) const {
         return this->name > rhs;
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
      bool operator<(const Enumeration &rhs) const {
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
      Enumeration(const std::string &name) {
         this->name = name;
      }

   private:
      ///Private assignment operator to enforce enumeration storage by reference.
      Enumeration &operator=(const Enumeration &rhs) { return *this; }

      ///String representation of the enumeration.
      std::string name;
      ///Private copy constructor to enforce enumeration storage by reference.
      Enumeration(const Enumeration &rhs) { }

      ///Helper method to print enumerations to an output stream.
      friend std::ostream &operator<<(std::ostream &os,
         const Enumeration &e)
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
   static std::vector<dtUtil::Enumeration *> instances;           \
   static void AddInstance(dtUtil::Enumeration *instance) {       \
   EnumType::instances.push_back(instance);        \
   }                                                   \
public:                                                 \
   static const std::vector<dtUtil::Enumeration *> &Enumerate() {       \
   return EnumType::instances;                     \
   }                                                   \
   \
   static Enumeration* GetValueForName(const std::string& name) {\
   for(unsigned i = 0; i < instances.size(); i++) { \
   if(name == instances[i]->GetName()) { \
   return instances[i];              \
   }                                     \
   }                                         \
   return NULL;                              \
   }

#define IMPLEMENT_ENUM(EnumType)                        \
   std::vector<dtUtil::Enumeration *> EnumType::instances;

}

#endif
