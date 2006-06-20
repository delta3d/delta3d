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
#ifndef DELTA_LIBRARYSHARINGMANAGER
#define DELTA_LIBRARYSHARINGMANAGER

#include <set>
#include <string>
#include <osg/Referenced>
#include <osgDB/DynamicLibrary>
#include <dtUtil/export.h>
#include <dtCore/refptr.h>
#include "dtUtil/exception.h"
#include "dtUtil/enumeration.h"

namespace dtUtil
{
   /**
    * @class LibrarySharingManager
    * @brief Singleton for controlling loading and unloading libraries shared by mulitple bodies of code.
    */
   class DT_UTIL_EXPORT LibrarySharingManager : public osg::Referenced
   {
      public:
         static LibrarySharingManager& GetInstance() 
         {
            if (!mInstance.valid())
               mInstance = new LibrarySharingManager;
            return *mInstance;
         }

         class DT_UTIL_EXPORT ExceptionEnum : public dtUtil::Enumeration
         {
               DECLARE_ENUM(ExceptionEnum);
            public:
               static ExceptionEnum LibraryLoadingError;        
            protected:
               ExceptionEnum(const std::string &name) : Enumeration(name)
               {
                  AddInstance(this);
               }
         };
         
         /**
          * @class LibraryHandle
          * @brief pure virtual class abstracting a checked-out handle to a library.  It has both a DynamicLibrary and
          * the system independent name of the library.
          */
         class LibraryHandle : public osg::Referenced
         {
            public:               
               typedef void* HANDLE;
               typedef void* SYMBOL_ADDRESS;
               
               virtual HANDLE GetHandle() const = 0;
               
               /**
                * @return The address of the given symbol or NULL if it was not found.
                */
               virtual SYMBOL_ADDRESS FindSymbol(const std::string& symbolName) const = 0;
               
               /**
                * @return the system-independent name of the library.
                */
               virtual const std::string& GetLibName() const = 0;
               
            protected:
               LibraryHandle() {}
               virtual ~LibraryHandle() {}
               void release();
               bool IsShuttingDown() const; 
            private:
               
               // -----------------------------------------------------------------------
               //  Unimplemented constructors and operators
               // -----------------------------------------------------------------------
               LibraryHandle(const LibraryHandle&) {}
               LibraryHandle& operator=(const LibraryHandle&) { return *this; }
         };
         
         /**
          * Loads a library based on a system independent name.  The class holds onto already opened 
          * libraries, so if the library is being used already, a new handle to it will be 
          * returned and no system calls will be made.
          * @param libName the system-independent name of the library to load.
          * @return a pointer to a handle representing the library.
          * @throws dtUtil::Exception with key dtUtil::LibrarySharingManager::ExceptionEnum::LibraryLoadingError if the library
          *         can't be loaded for some reason.
          */
         dtCore::RefPtr<LibraryHandle> LoadSharedLibrary(const std::string& libName) 
            throw(dtUtil::Exception);
                  
         /**
          * Determines which platform we are running on and returns a
          * platform dependent library name.
          * @param libBase Platform independent library name.
          * @return A platform dependent library name.
          * @note
          *  For example.  If the platform independent library name is
          *  ExampleActors then on Windows platforms the resulting dependent
          *  library name would be ExampleActors.dll, however, on Unix based
          *  platforms, the resulting name would be libExampleActors.so.
          */
         static std::string GetPlatformSpecificLibraryName(const std::string &libBase) throw();
  
         /**
          * Strips off the path and platform specific library prefixs and extensions
          * and returns a system independent file name.
          * @param libName The platform specific library name.
          * @return A platform independent library name.
          */
         static std::string GetPlatformIndependentLibraryName(const std::string &libName) throw();

      private:
         static dtCore::RefPtr<LibrarySharingManager> mInstance;

         LibrarySharingManager();
         virtual ~LibrarySharingManager();
         //map of the platform independent name to the actual library
         std::map<std::string, dtCore::RefPtr<LibraryHandle> > mLibraries;

         bool mShuttingDown;

         /**
          * This releases the handle to a loaded library.  If there are no other handles to 
          * referenced library in use, the library will be closed.
          * @param a pointer to the handle to release.  This should not be NULL.
          */
         void ReleaseSharedLibraryHandle(LibraryHandle* handle) throw();


         // -----------------------------------------------------------------------
         //  Unimplemented constructors and operators
         // -----------------------------------------------------------------------
         LibrarySharingManager(const LibrarySharingManager&);
         LibrarySharingManager& operator=(const LibrarySharingManager&);
   };

}

#endif /*DELTA_LIBRARYSHARINGMANAGER*/
