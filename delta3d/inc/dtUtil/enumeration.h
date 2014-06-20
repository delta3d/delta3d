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
 * David Guthrie
 */

#ifndef DELTA_ENUMERATION
#define DELTA_ENUMERATION

////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <iosfwd>
#include <dtUtil/export.h>

#include <dtUtil/warningdisable.h>
//Disable visual C++ compiler warnings that seem to indicate the compiler is
//getting confused when compiling an enumeration.
DT_DISABLE_WARNING_START_MSVC(4276)

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
       * Inlined because it's called frequently
       * @return the string representation of this enumeration.
       */
      const std::string& GetName() const
      {
         return mName;
      }

      /**
       * Equality test for an enumeration.  Since enumeration objects are static,
       * and only references to enumerations may be stored by the user, it is
       * safe and efficient to compare enumeration objects based on their memory
       * address.
       * Inlined because it's called frequently
       */
      bool operator==(const Enumeration& rhs) const
      {
         return this == &rhs;
      }

      /**
       * Inequality test for an enumeration.
       * Inlined because it's called frequently
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
      bool operator==(const std::string& rhs) const;

      ///Overloaded inequality test for this enumeration's string value.
      bool operator!=(const std::string& rhs) const;

      ///Overloaded less than test for this enumeration's string value.
      bool operator<(const std::string& rhs) const;

      ///Overloaded greater than test for this enumeration's string value.
      bool operator>(const std::string& rhs) const;

      /**
       * Overloaded less than operator.  This checks the memory addresses of the two
       * enumerations.
       * Inlined because it's called frequently
       * @param rhs The second enumeration to compare to this one.
       * @return True if this enumeration is less than rhs.
       * @note
       *  This method is supported by enumerations so they may be used as keys
       *  in STL containers.  Since memory addresses are guaranteed to be unique
       *  this methods works fine.  However, it really does not make sense to use
       *  this method of comparison in other circumstances.
       */
      bool operator<(const Enumeration& rhs) const
      {
         return this < &rhs;
      }

   protected:
      /**
       * virtual string compare method to used by the operators that take a string.  This allows overriding the behavior in a subclass.
       *  It should work like std::string::compare
       */
      virtual int Compare(const std::string& nameString) const;

      ///Private virtual desctructor to get rid of compile warning
      virtual ~Enumeration();

      /**
       * Construct the enumeration.
       *  @note
       *      When creating a new enumeration, the constructor of derived types
       *      must call addInstance(this) in order for enumerations to be
       *      enumerated.
       */
      Enumeration(const std::string& name);

   private:
      ///Private assignment operator to enforce enumeration storage by reference.
      Enumeration& operator=(const Enumeration&);

      ///String representation of the enumeration.
      std::string mName;
      ///Private copy constructor to enforce enumeration storage by reference.
      Enumeration(const Enumeration&);

   };

   ///Helper method to print enumerations to an output stream.
   DT_UTIL_EXPORT std::ostream& operator<<(std::ostream& os, const Enumeration& e);

/**
 * Helper macros used to create the static data and methods
 * needed to enumerate an enumeration.
 */
#define DECLARE_ENUM(EnumType)                                 \
public:                                                        \
   typedef std::vector<EnumType*> EnumerateListType;           \
                                                               \
   static const EnumerateListType& EnumerateType()             \
   {                                                           \
      return EnumType::mInstances;                             \
   }                                                           \
                                                               \
   static const std::vector<dtUtil::Enumeration*>& Enumerate() \
   {                                                           \
      return EnumType::mGenericInstances;                      \
   }                                                           \
                                                               \
   static EnumType* GetValueForName(const std::string& name);  \
                                                               \
private:                                                       \
   static EnumerateListType mInstances;                        \
   static std::vector<dtUtil::Enumeration*> mGenericInstances; \
   static void AddInstance(EnumType* instance);                \
public:


#define IMPLEMENT_ENUM(EnumType)                               \
   EnumType::EnumerateListType EnumType::mInstances;           \
   std::vector<dtUtil::Enumeration*> EnumType::mGenericInstances; \
   void EnumType::AddInstance(EnumType* instance)                 \
   {                                                           \
      EnumType::mInstances.push_back(instance);                \
      EnumType::mGenericInstances.push_back(instance);         \
   }                                                           \
   EnumType* EnumType::GetValueForName(const std::string& name)   \
   {                                                           \
      for (unsigned i = 0; i < mInstances.size(); ++i)         \
      {                                                        \
         if ((*mInstances[i]) == name)                 \
         {                                                     \
            return mInstances[i];                              \
         }                                                     \
      }                                                        \
      return NULL;                                             \
   }

   template <typename T>
   class EnumerationPointer
   {
   public:
      typedef T element_type;
      EnumerationPointer() : mEnum(NULL) {}
      EnumerationPointer(T* ptr) : mEnum(ptr) { }
      EnumerationPointer(const EnumerationPointer& rp) : mEnum(rp.mEnum) { }
      template<class Other> EnumerationPointer(const EnumerationPointer<Other>& rp) : mEnum(rp.mEnum) { }

      ~EnumerationPointer() { mEnum = NULL; }

      operator T*() const { return mEnum; }
      operator T&() const { return *mEnum; }

      EnumerationPointer& operator = (const EnumerationPointer& rp)
      {
         mEnum = rp.mEnum;
         return *this;
      }

      template<class Other> EnumerationPointer& operator = (const EnumerationPointer<Other>& rp)
      {
         mEnum = rp.mEnum;
         return *this;
      }

      inline EnumerationPointer& operator = (T* ptr)
      {
         if (mEnum==ptr) return *this;
         mEnum = ptr;
         return *this;
      }

      inline EnumerationPointer& operator = (T& enumRef)
      {
         if (mEnum==&enumRef) return *this;
         mEnum = &enumRef;
         return *this;
      }

      // comparison operators for EnumerationPointer.
      bool operator == (const EnumerationPointer& rp) const { return (mEnum==rp.mEnum); }
      bool operator == (const T* ptr) const { return (mEnum==ptr); }
      friend bool operator == (const T* ptr, const EnumerationPointer& rp) { return (ptr==rp.mEnum); }

      bool operator != (const EnumerationPointer& rp) const { return (mEnum!=rp.mEnum); }
      bool operator != (const T* ptr) const { return (mEnum!=ptr); }
      friend bool operator != (const T* ptr, const EnumerationPointer& rp) { return (ptr!=rp.mEnum); }

      bool operator < (const EnumerationPointer& rp) const { return (mEnum<rp.mEnum); }

      T& operator*() const { return *mEnum; }
      T* operator->() const { return mEnum; }
      T* get() const { return mEnum; }

      bool operator!() const   { return mEnum==0; } // not required
      bool valid() const       { return mEnum!=0; }

      void swap(EnumerationPointer& rp) { T* tmp=mEnum; mEnum=rp.mEnum; rp.mEnum=tmp; }
   private:
      T* mEnum;
   };


} // namespace dtUtil



DT_DISABLE_WARNING_END

////////////////////////////////////////////////////////////////////////////////

#endif // DELTA_ENUMERATION
