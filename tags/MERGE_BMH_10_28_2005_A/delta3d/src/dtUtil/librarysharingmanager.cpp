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
 * @author David Guthrie
 */
#include <sstream>
#include <osgDB/FileNameUtils>
#include "dtUtil/librarysharingmanager.h"

namespace dtUtil
{
   IMPLEMENT_ENUM(LibrarySharingManager::ExceptionEnum);

   LibrarySharingManager::ExceptionEnum LibrarySharingManager::ExceptionEnum::LibraryLoadingError("Base Exception");

   void LibrarySharingManager::LibraryHandle::release() 
   {
      //Tell this manager the handle is being deleted.
      LibrarySharingManager::GetInstance().ReleaseSharedLibraryHandle(this);
      
   }  
   
   class InternalLibraryHandle: public LibrarySharingManager::LibraryHandle 
   {
      public:
         InternalLibraryHandle(const std::string& libName, osgDB::DynamicLibrary& lib): 
            LibrarySharingManager::LibraryHandle(), mLibName(libName), mLibrary(&lib) {}
         
         virtual const std::string& GetLibName() const
         {
            return mLibName;
         } 

         virtual osgDB::DynamicLibrary& GetDynamicLibrary()
         {
            return *mLibrary;
         }

         virtual const osgDB::DynamicLibrary& GetDynamicLibrary() const 
         {
            return *mLibrary;
         }
         
         virtual ~InternalLibraryHandle() 
         {
            //break the handle
            mLibrary = NULL;
            release();
         }
                  
      private:
         std::string mLibName;
         dtCore::RefPtr<osgDB::DynamicLibrary> mLibrary;
                      
   };


   dtCore::RefPtr<LibrarySharingManager> LibrarySharingManager::mInstance;

   ///////////////////////////////////////////////////////////////////////////////
   LibrarySharingManager::LibrarySharingManager()
   {
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   LibrarySharingManager::~LibrarySharingManager()
   {
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   LibrarySharingManager::LibrarySharingManager(const LibrarySharingManager&){}
   ///////////////////////////////////////////////////////////////////////////////
   LibrarySharingManager& LibrarySharingManager::operator=(const LibrarySharingManager&)
   {
      return *this;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<LibrarySharingManager::LibraryHandle> LibrarySharingManager::LoadSharedLibrary(const std::string& libName) 
   throw(dtUtil::Exception)
   {
      osgDB::DynamicLibrary *dynLib;
        
        
      std::map<std::string, dtCore::RefPtr<osgDB::DynamicLibrary> >::iterator itor = mLibraries.find(libName);
      if (itor == mLibraries.end())
      {
         std::string actualLibName;

         //Get the system dependent name of the library.
         actualLibName = GetPlatformSpecificLibraryName(libName);
         std::ostringstream msg;
         //First, try and load the dynamic library.
         msg << "Loading library " << actualLibName;
         LOG_INFO(msg.str());
         dynLib = osgDB::DynamicLibrary::loadLibrary(actualLibName);
         if (dynLib == NULL)
         {
            msg.clear();
            msg.str("");
            msg << "Unable to library " << actualLibName;
            LOG_ERROR(msg.str());
            EXCEPT(LibrarySharingManager::ExceptionEnum::LibraryLoadingError, msg.str());  
         }
         else
         {
            itor = mLibraries.insert(std::make_pair(libName, dtCore::RefPtr<osgDB::DynamicLibrary>(dynLib))).first;
         }

      }

      return new InternalLibraryHandle(itor->first, *itor->second);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void LibrarySharingManager::ReleaseSharedLibraryHandle(LibraryHandle* handle) throw()
   {
      if (handle == NULL)
         return;
         
      std::map<std::string, dtCore::RefPtr<osgDB::DynamicLibrary> >::iterator itor = mLibraries.find(handle->GetLibName());
      if (itor != mLibraries.end()) 
      {
         //if the the reference in the data structure is the last reference, then erase the library, causing it to be
         //uploaded.
         if (itor->second->referenceCount() == 1)
            mLibraries.erase(itor);
      }      
   }

   ///////////////////////////////////////////////////////////////////////////////
   std::string LibrarySharingManager::GetPlatformSpecificLibraryName(const std::string &libBase) throw()
   {
      #if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
         return libBase + ".dll";
      #elif defined(__APPLE__)
         return "lib" + libBase + ".so";
      #else
         return "lib" + libBase + ".so";
      #endif
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   std::string LibrarySharingManager::GetPlatformIndependentLibraryName(const std::string &libName) throw()
   {
      std::string iName = osgDB::getStrippedName(libName);
   
      #if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
         return iName;
      #else
         return std::string(iName.begin()+3,iName.end());
      #endif
   }
}
