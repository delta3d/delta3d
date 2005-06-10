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
#ifndef __ObjectFactory__h
#define __ObjectFactory__h

#include <map>
#include <sstream>
//#include "Log.h"
//#include "Exception.h"
#include "osg/Referenced"  // for base_class

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
   *	The ObjectFactory implementation only supports objects with
   *	a default constructor.  It will not work with objects that
   *	only have named constructors.
   */
   template<typename UniqueIdType,typename BaseType,class ltCmp=std::less<UniqueIdType> >
   class ObjectFactory : public osg::Referenced
   {
   public:
      typedef BaseType *(*createObjectFunc)(); /// Function pointer type for functions creating objects.
      typedef std::map<UniqueIdType,createObjectFunc,ltCmp> ObjectMap;
      typedef typename ObjectMap::iterator ObjTypeItor;
      ObjectFactory() {}  // constructor

   protected:
      virtual ~ObjectFactory() {}

   public:
      /**
      * Registers a new type of object with the factory.
      */
      template<typename DerivedType>
      bool RegisterType(UniqueIdType id)
      {
         if (this->objectTypeMap.find(id) != this->objectTypeMap.end())
         {
            std::ostringstream ss;
            ss << "Duplicate object type " << id << " found.";
            //LOG_ERROR(ss.str());
            return false;
         }

         this->objectTypeMap[id] = &construct<BaseType,DerivedType>;
         return true;
      }

      /**
      * Removes an existing object type from the factory's known list
      * of object types.
      */
      void RemoveType(UniqueIdType id) {
         if (this->objectTypeMap.erase(id) != 1) {
            std::ostringstream ss;
            ss << "ID " << id << " cannot be removed because it does not exist.";
            //LOG_WARNING(ss.str());
         }
      }

      /**
      * Checks to see if the factory can create objects of the given type.
      * @param id The type of object to check for.
      * @return True if the type is supported, false otherwise.
      */
      bool IsTypeSupported(UniqueIdType id) {
         ObjTypeItor itor(this->objectTypeMap.find(id));
         if (itor != this->objectTypeMap.end())
            return true;
         else
            return false;
      }

      /**
      * Gets a list of types that this factory knows how to create.
      */
      void GetSupportedTypes(std::vector<UniqueIdType> &types) {
         types.clear();
         for (ObjTypeItor itor=this->objectTypeMap.begin();
            itor != this->objectTypeMap.end(); ++itor) {
               types.push_back(itor->first);
            }
      }

      /**
      * Creates a new object.
      * @param id - Type of object to create.
      * @return Returns a pointer to the newly created object.
      * @throw Exception is thrown if the factory does not know how to create
      *  the requested type.
      */
      BaseType *CreateObject(UniqueIdType id)
      {
         ObjTypeItor itor(this->objectTypeMap.find(id));

         //We cannot create a new object if we do not know what type it is
         //so throw an exception.
         if (itor == this->objectTypeMap.end())
         {
            std::ostringstream ss;
            ss << "Unable to create object of type: " << id << " Reason: Unknown type.";
            //EXCEPT(ExceptionEnum::ObjectFactoryUnknownType,ss.str());
            return 0;
         }

         return (itor->second)();
      }

      const ObjectMap& GetMap() const { return objectTypeMap; }
      ObjectMap& GetMap()             { return objectTypeMap; }

   private:
      ///Maps a unique id to a function pointer that when called creates an
      ///object of the appropriate type.
      ObjectMap objectTypeMap;
   };
}

#endif
