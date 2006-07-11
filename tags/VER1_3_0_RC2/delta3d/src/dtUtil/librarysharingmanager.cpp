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

#if defined(WIN32) && !defined(__CYGWIN__)
  #include <Io.h>
  #include <Windows.h>
  #include <Winbase.h>
#else
  #include <dlfcn.h>
#endif

#include <sstream>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include "dtUtil/librarysharingmanager.h"

namespace dtUtil
{
   IMPLEMENT_ENUM(LibrarySharingManager::ExceptionEnum);

   LibrarySharingManager::ExceptionEnum LibrarySharingManager::ExceptionEnum::LibraryLoadingError("Base Exception");

   bool LibrarySharingManager::LibraryHandle::IsShuttingDown() const
   {
      return (!LibrarySharingManager::mInstance.valid()) || LibrarySharingManager::mInstance->mShuttingDown;
   } 

   void LibrarySharingManager::LibraryHandle::release()
   {
      if (!IsShuttingDown())
         LibrarySharingManager::GetInstance().ReleaseSharedLibraryHandle(this);
      
   }  
   
   class InternalLibraryHandle : public LibrarySharingManager::LibraryHandle 
   {
      public:
         InternalLibraryHandle(const std::string& libName, HANDLE libHandle, bool close): 
            LibrarySharingManager::LibraryHandle(), mLibName(libName), mHandle(libHandle), mClose(close)  {}
         
         ///Loads the library and returns a ref pointer to it.  This will not reload libraries that are already
         ///loaded.
         static dtCore::RefPtr<LibrarySharingManager::LibraryHandle> LoadLibrary(const std::string& libraryName)
         {
            HANDLE handle = NULL;
            
            std::string fullLibraryName = osgDB::findLibraryFile(libraryName);            
            if (fullLibraryName.empty())
               fullLibraryName = libraryName;
            
            //close is only used in windows so that it can be false if the lib is already
            //loaded and a handle is found for it.  Freeing a library that is linked it at
            //compile time would be bad.
            bool close = true;
            
#if defined(WIN32) && !defined(__CYGWIN__)
            //see if the library is already loaded because windows will load libraries multiple times.
            handle = GetModuleHandle( libraryName.c_str() );
            if (handle != NULL)
               close = false;
            else
               handle = ::LoadLibrary( fullLibraryName.c_str() );
            
            if (handle == NULL)
               LOG_ERROR("Unable to load library \"" + fullLibraryName + ".\"");

#else
            // dlopen will not work with files in the current directory unless
            // they are prefaced with './'  (DB - Nov 5, 2003).
            std::string localLibraryName;
            if( fullLibraryName == osgDB::getSimpleFileName( fullLibraryName ) )
               localLibraryName = "./" + fullLibraryName;
            else
               localLibraryName = fullLibraryName;
            
            handle = dlopen( localLibraryName.c_str(), RTLD_LAZY | RTLD_GLOBAL);
            if( handle == NULL )
               LOG_ERROR("Error loading library \"" + fullLibraryName + "\" with dlopen(): " + dlerror());
            
#endif
            if (handle != NULL) return new InternalLibraryHandle(fullLibraryName, handle, close);
            
            return NULL;
         }
         
         virtual LibrarySharingManager::LibraryHandle::HANDLE GetHandle() const 
         {
            return mHandle;
         }

         ///Looks up a function symbol
         virtual LibrarySharingManager::LibraryHandle::SYMBOL_ADDRESS FindSymbol(const std::string& symbolName) const
         {
            if (mHandle==NULL) return NULL;
#if defined(WIN32) && !defined(__CYGWIN__)
            return (LibraryHandle::SYMBOL_ADDRESS)GetProcAddress( (HMODULE)mHandle,
                                                                 symbolName.c_str() );
#else
            void* sym = dlsym( mHandle,  symbolName.c_str() );
            if (sym == NULL) {
               LOG_WARNING("Failed looking up \"" + symbolName + "\" in library \"" + GetLibName() + "\": " + dlerror() );
            }
            return sym;
#endif            
         }
         
         virtual const std::string& GetLibName() const
         {
            return mLibName;
         } 

      protected:
         virtual ~InternalLibraryHandle() 
         {
            if (mHandle != NULL)
            {
               if (!IsShuttingDown())
                  LOG_INFO("Closing DynamicLibrary: " + mLibName);
#if defined(WIN32) && !defined(__CYGWIN__)
               if (mClose)
                  FreeLibrary((HMODULE)mHandle);
#else // other unix
               dlclose(mHandle);
#endif    
            }
            release();
         }
      
      private:
         std::string mLibName;
         HANDLE mHandle;
         bool mClose;
         /// disable default constructor.
         InternalLibraryHandle(): LibrarySharingManager::LibraryHandle()  {}
         /// disable copy constructor.
         InternalLibraryHandle(const InternalLibraryHandle&): LibrarySharingManager::LibraryHandle()  {}
         /// disable copy operator.
         InternalLibraryHandle& operator=(const InternalLibraryHandle&) { return *this; }
                  
   };


   dtCore::RefPtr<LibrarySharingManager> LibrarySharingManager::mInstance;

   ///////////////////////////////////////////////////////////////////////////////
   LibrarySharingManager::LibrarySharingManager(): mShuttingDown(false)
   {
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   LibrarySharingManager::~LibrarySharingManager()
   {
      mShuttingDown = true;
      mLibraries.clear();
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
      if (mShuttingDown)
         EXCEPT(LibrarySharingManager::ExceptionEnum::LibraryLoadingError, "Library Manager is shutting down.");  
         
      dtCore::RefPtr<LibrarySharingManager::LibraryHandle> dynLib;
        
        
      std::map<std::string, dtCore::RefPtr<LibrarySharingManager::LibraryHandle> >::iterator itor = mLibraries.find(libName);
      if (itor == mLibraries.end())
      {
         std::string actualLibName;

         //Get the system dependent name of the library.
         actualLibName = GetPlatformSpecificLibraryName(libName);
         std::ostringstream msg;
         //First, try and load the dynamic library.
         msg << "Loading library " << actualLibName;
         LOG_INFO(msg.str());
         dynLib = InternalLibraryHandle::LoadLibrary(actualLibName);
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
            itor = mLibraries.insert(std::make_pair(libName, dtCore::RefPtr<LibrarySharingManager::LibraryHandle>(dynLib))).first;
         }

      }
      else
      {
         dynLib = itor->second;
      }
      
      if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::ostringstream ss;
         ss << dynLib->referenceCount();
         LOG_DEBUG(ss.str());         
      }
      
      return dynLib;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void LibrarySharingManager::ReleaseSharedLibraryHandle(LibraryHandle* handle) throw()
   {
      if (handle == NULL)
         return;

      if (mShuttingDown)
         return;
         
      std::map<std::string, dtCore::RefPtr<LibrarySharingManager::LibraryHandle> >::iterator itor = mLibraries.find(handle->GetLibName());
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
         #ifdef _DEBUG
         return libBase + "d.dll";
         #else
         return libBase + ".dll";
         #endif
      #elif defined(__APPLE__)
         return "lib" + libBase + ".dylib";
      #else
         return "lib" + libBase + ".so";
      #endif
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   std::string LibrarySharingManager::GetPlatformIndependentLibraryName(const std::string &libName) throw()
   {
      std::string iName = osgDB::getStrippedName(libName);
   
      #if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
         #ifdef _DEBUG
         //Pull off the final "d"
         return std::string(iName.begin(),iName.end() - 1);
         #else
         return iName;
         #endif
      #else
         return std::string(iName.begin()+3,iName.end());
      #endif
   }
}
