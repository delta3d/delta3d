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

#ifndef DELTA_OBJECT_FACTORY
#define DELTA_OBJECT_FACTORY

#include <map>
#include <vector>
#include <osg/Referenced>  // for base_class

namespace dtUtil
{
   /**
    * Templated function to provide a generic object construction
    * utility.
    */
   template<typename BaseType, typename DerivedType>
   BaseType *construct()
   {
      return new DerivedType();
   }

   /**
    * This class is a template object factory.  It allows one to
    * create any type of object as long as there is a common base
    * class.  The common base class is defined on a per-factory
    * basis using the templated parameter <code>BaseType</code>.
    * @note
    *   The ObjectFactory implementation only supports objects with
    *   a default constructor.  It will not work with objects that
    *   only have named constructors.
    */
   template<typename UniqueIdTypeClass,typename BaseTypeClass,typename ltCmpClass=std::less<UniqueIdTypeClass> >
   class ObjectFactory : public osg::Referenced
   {
   public:

      typedef UniqueIdTypeClass UniqueIdType;
      typedef BaseTypeClass BaseType;
      typedef ltCmpClass ltCmp;

      typedef BaseType *(*createObjectFunc)(); /// Function pointer type for functions creating objects.
      typedef std::map<UniqueIdType,createObjectFunc,ltCmp> ObjectMap;
      typedef typename ObjectMap::iterator ObjTypeItor;
      typedef typename ObjectMap::const_iterator ObjTypeItorConst;
      ObjectFactory() {}  // constructor

   protected:
      virtual ~ObjectFactory() {}

   public:
      /**
       * Registers a new type of object with the factory.
       * @return false if the type is a duplicate.
       */
      template<typename DerivedType>
      bool RegisterType(UniqueIdType id)
      {
         if (this->objectTypeMap.find(id) != this->objectTypeMap.end())
         {
            return false;
         }

         this->objectTypeMap[id] = &construct<BaseType,DerivedType>;
         return true;
      }

      /**
       * Registers a new type of object with the factory.
       * This is version assumes that a static constant named TYPE is declared on the class.
       * @return false if the type is a duplicate.
       */
      template<typename DerivedType>
      bool RegisterType()
      {
         if (this->objectTypeMap.find(DerivedType::TYPE.get()) != this->objectTypeMap.end())
         {
            return false;
         }

         this->objectTypeMap[DerivedType::TYPE.get()] = &construct<BaseType,DerivedType>;
         return true;
      }

      /**
       * Removes an existing object type from the factory's known list
       * of object types.
       */
      void RemoveType(UniqueIdType id) {
         this->objectTypeMap.erase(id);
      }

      /**
       * Checks to see if the factory can create objects of the given type.
       * @param id The type of object to check for.
       * @return True if the type is supported, false otherwise.
       */
      bool IsTypeSupported(UniqueIdType id) const
      {
         ObjTypeItorConst itor(this->objectTypeMap.find(id));
         if (itor != this->objectTypeMap.end())
         {
            return true;
         }
         else
         {
            return false;
         }
      }

      /**
       * Gets a list of types that this factory knows how to create.
       * It will add to the vector you pass it without clearing.
       */
      void GetSupportedTypes(std::vector<UniqueIdType>& addToVec) const
      {
         addToVec.reserve(addToVec.size() + objectTypeMap.size());
         for (ObjTypeItorConst itor=this->objectTypeMap.begin();
            itor != this->objectTypeMap.end(); ++itor)
         {
            addToVec.push_back(itor->first);
         }
      }

      /**
       * Creates a new object.
       * @param id - Type of object to create.
       * @return Returns a pointer to the newly created object or NULL if the given id has not been registered.
       * @throw Exception is thrown if the factory does not know how to create
       *  the requested type.
       */
      BaseType* CreateObject(const UniqueIdType id) const
      {
         ObjTypeItorConst itor(this->objectTypeMap.find(id));

         // We cannot create a new object if we do not know what type it is
         // so throw an exception.
         if (itor == this->objectTypeMap.end())
         {
            return NULL;
         }

         return (itor->second)();
      }

      const ObjectMap& GetMap() const { return objectTypeMap; }

   private:
      ///Maps a unique id to a function pointer that when called creates an
      ///object of the appropriate type.
      ObjectMap objectTypeMap;
   };
}

#endif
