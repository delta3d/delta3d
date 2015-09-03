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
 * David Guthrie
 */
#include <prefix/dtutilprefix.h>
#include <dtUtil/mswinmacros.h>

#if defined(DELTA_WIN32) && !defined(__CYGWIN__)
  #include <dtUtil/mswin.h>
#else
  #include <dlfcn.h>
#endif

#include <sstream>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

#include <dtUtil/fileutils.h>
#include <dtUtil/librarysharingmanager.h>

using std::string;
using std::set;

namespace dtUtil
{
   bool LibrarySharingManager::LibraryHandle::IsShuttingDown() const
   {
      return (!LibrarySharingManager::mInstance.valid()) || LibrarySharingManager::mInstance->mShuttingDown;
   }

   void LibrarySharingManager::LibraryHandle::release()
   {
      if (!IsShuttingDown())
      {
         LibrarySharingManager::GetInstance().ReleaseSharedLibraryHandle(this);
      }
   }

   class InternalLibraryHandle : public LibrarySharingManager::LibraryHandle
   {
   public:
      InternalLibraryHandle(const string& libName, HANDLE libHandle, bool close):
         LibrarySharingManager::LibraryHandle(), mLibName(libName), mHandle(libHandle), mClose(close)  {}

      ///Loads the library and returns a ref pointer to it.  This will not reload libraries that are already
      ///loaded.
      static dtCore::RefPtr<LibrarySharingManager::LibraryHandle> LoadSharedLibrary(const string& libraryName)
      {
         HANDLE handle  = NULL;

         string fullLibraryName = osgDB::findLibraryFile(libraryName);
         if (fullLibraryName.empty())
         {
            fullLibraryName = LibrarySharingManager::GetInstance().FindLibraryInSearchPath(libraryName);

            // if It's still empty, just set it to the plain name and
            // use the OS search path.
            if (fullLibraryName.empty())
            {
               fullLibraryName = libraryName;
            }
         }

         // close is only used in windows so that it can be false if the lib is already
         // loaded and a handle is found for it.  Freeing a library that is linked it at
         // compile time would be bad.
         bool close = true;

#if defined(WIN32) && !defined(__CYGWIN__)
         // Make sure the error state is set to a non-garbage value
         SetLastError(0);

         // see if the library is already loaded because windows will load libraries multiple times.
         handle = GetModuleHandle((LPCSTR)libraryName.c_str());
         if (handle != NULL)
         {
            close = false;
         }
         else
         {
            handle = LoadLibrary(fullLibraryName.c_str());
         }

         if (handle == NULL)
         {
            LPVOID lpMsgBuf;
            FormatMessage(
               FORMAT_MESSAGE_ALLOCATE_BUFFER |
               FORMAT_MESSAGE_FROM_SYSTEM |
               FORMAT_MESSAGE_IGNORE_INSERTS,
               NULL,
               GetLastError(),
               0, // Default language
               (LPTSTR) &lpMsgBuf,
               0,
               NULL
               );

            std::string errorDisplay = std::string(reinterpret_cast<char*>(lpMsgBuf));
            // Display the string.
            LOG_ERROR("Unable to load library \"" + fullLibraryName + "\":" + errorDisplay);
            // Free the buffer.
            LocalFree(lpMsgBuf);
         }
#else
         // dlopen will not work with files in the current directory unless
         // they are prefaced with './'  (DB - Nov 5, 2003).

         std::string libraryTempString;
#ifdef __APPLE__
         libraryTempString = "@executable_path/../lib/" + fullLibraryName;
         LOGN_DEBUG("librarysharingmanager.cpp", "Attempting to load library: " + libraryTempString);
         handle = dlopen((libraryTempString).c_str(), RTLD_LAZY | RTLD_GLOBAL);
         if (handle == NULL)
         {
            libraryTempString = "@executable_path/../PlugIns/" + fullLibraryName;
            LOGN_DEBUG("librarysharingmanager.cpp", "Attempting to load library: " + libraryTempString);
            handle = dlopen((libraryTempString).c_str(), RTLD_LAZY | RTLD_GLOBAL);
         }
#endif
         if (handle == NULL)
         {
            libraryTempString = "./" + fullLibraryName;
            LOGN_DEBUG("librarysharingmanager.cpp", "Attempting to load library: " + libraryTempString);
            handle = dlopen((libraryTempString).c_str(), RTLD_LAZY | RTLD_GLOBAL);
         }

         if (handle == NULL)
         {
            handle = dlopen(fullLibraryName.c_str(), RTLD_LAZY | RTLD_GLOBAL);
         }

         if (handle == NULL)
         {
            LOG_ERROR("Error loading library \"" + fullLibraryName + "\" with dlopen(): " + dlerror());
         }
#endif
         if (handle != NULL)
         {
             return new InternalLibraryHandle(fullLibraryName, handle, close);
         }

         return NULL;
      }

      virtual LibrarySharingManager::LibraryHandle::HANDLE GetHandle() const
      {
         return mHandle;
      }

      ///Looks up a function symbol
      virtual LibrarySharingManager::LibraryHandle::SYMBOL_ADDRESS FindSymbol(const string& symbolName) const
      {
         if (mHandle == NULL) { return NULL; }
#if defined(WIN32) && !defined(__CYGWIN__)
         return (LibraryHandle::SYMBOL_ADDRESS)GetProcAddress((HMODULE)mHandle,
                                                              symbolName.c_str());
#else
         void* sym = dlsym(mHandle,  symbolName.c_str());
         if (sym == NULL)
         {
            LOG_WARNING("Failed looking up \"" + symbolName + "\" in library \"" + GetLibName() + "\": " + dlerror());
         }
         return sym;
#endif
      }

      virtual const string& GetLibName() const
      {
         return mLibName;
      }

   protected:
      virtual ~InternalLibraryHandle()
      {
         if (mHandle != NULL)
         {
            if (!IsShuttingDown())
            {
               LOG_INFO("Closing DynamicLibrary: " + mLibName);
            }
#if defined(WIN32) && !defined(__CYGWIN__)
            if (mClose)
            {
               FreeLibrary((HMODULE)mHandle);
            }
#else // other unix
            dlclose(mHandle);
#endif
         }
         release();
      }

   private:
      string mLibName;
      HANDLE mHandle;
DT_DISABLE_WARNING_START_CLANG("-Wunused-private-field")
      bool mClose;
DT_DISABLE_WARNING_END
      /// disable default constructor.
      InternalLibraryHandle(): LibrarySharingManager::LibraryHandle(), mHandle(0), mClose(false) {}
      /// disable copy constructor.
      InternalLibraryHandle(const InternalLibraryHandle&): LibrarySharingManager::LibraryHandle(), mHandle(0), mClose(false) {}
      /// disable copy operator.
      InternalLibraryHandle& operator=(const InternalLibraryHandle&) { return *this; }
   };

   dtCore::RefPtr<LibrarySharingManager> LibrarySharingManager::mInstance;

   ///////////////////////////////////////////////////////////////////////////////
   LibrarySharingManager::LibrarySharingManager() : mShuttingDown(false)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   LibrarySharingManager::~LibrarySharingManager()
   {
      mShuttingDown = true;
      //mLibraries.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   LibrarySharingManager::LibrarySharingManager(const LibrarySharingManager&){}

   ///////////////////////////////////////////////////////////////////////////////
   LibrarySharingManager& LibrarySharingManager::operator=(const LibrarySharingManager&)
   {
      return *this;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<LibrarySharingManager::LibraryHandle> LibrarySharingManager::LoadSharedLibrary(const string& libName, bool assumeModule)
   {
      if (mShuttingDown)
      {
         throw dtUtil::LibrarySharingManager::LibraryLoadingException(
            "Library Manager is shutting down.", __FILE__, __LINE__);
      }

      dtCore::RefPtr<LibrarySharingManager::LibraryHandle> dynLib;


      std::map< string, dtCore::RefPtr<LibrarySharingManager::LibraryHandle> >::iterator itor = mLibraries.find(libName);
      if (itor == mLibraries.end())
      {
         string actualLibName;

         // Get the system dependent name of the library.
         actualLibName = GetPlatformSpecificLibraryName(libName, assumeModule);
         std::ostringstream msg;
         // First, try and load the dynamic library.
         msg << "Loading library " << actualLibName;
         LOG_INFO(msg.str());
         dynLib = InternalLibraryHandle::LoadSharedLibrary(actualLibName);

         if (dynLib == NULL)
         {
            std::string actualLibName2 = GetPlatformSpecificLibraryName(libName, !assumeModule);
            if (actualLibName != actualLibName2)
            {
               std::ostringstream msg2;
               // First, try to load the dynamic library.
               msg2 << "Re-attempting using the module extension: " << actualLibName2;
               LOG_ALWAYS(msg2.str());
               dynLib = InternalLibraryHandle::LoadSharedLibrary(actualLibName2);
            }
         }

         if (dynLib == NULL)
         {
            msg.clear();
            msg.str("");
            msg << "Unable to load library " << actualLibName;
            throw dtUtil::LibrarySharingManager::LibraryLoadingException(
               msg.str(), __FILE__, __LINE__);
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
   void LibrarySharingManager::ReleaseSharedLibraryHandle(LibraryHandle* handle)
   {
      if (handle == NULL)
      {
         return;
      }

      if (mShuttingDown)
      {
         return;
      }

      std::map<string, dtCore::RefPtr<LibrarySharingManager::LibraryHandle> >::iterator itor = mLibraries.find(handle->GetLibName());
      if (itor != mLibraries.end())
      {
         // if the the reference in the data structure is the last reference, then erase the library, causing it to be
         // uploaded.
         if (itor->second->referenceCount() == 1)
         {
            mLibraries.erase(itor);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void LibrarySharingManager::GetSearchPath(std::vector<string>& toFill) const
   {
      toFill.clear();
      toFill.reserve(mSearchPath.size());
      toFill.insert(toFill.begin(), mSearchPath.begin(), mSearchPath.end());
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string LibrarySharingManager::FindLibraryInSearchPath(const std::string& libraryFileName) const
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      string path;
      for (set<string>::const_iterator itor = mSearchPath.begin(); itor != mSearchPath.end(); ++itor)
      {
         path = *itor;
         // Make sure we remove any trailing slashes from the cache path.
         if (path[path.length()-1] == '/' || path[path.length()-1] == '\\')
         {
            path = path.substr(0, path.length()-1);
         }

         if (fileUtils.FileExists(path + dtUtil::FileUtils::PATH_SEPARATOR + libraryFileName))
         {
            return  path + dtUtil::FileUtils::PATH_SEPARATOR + libraryFileName;
         }
      }
      return string();
   }
   ///////////////////////////////////////////////////////////////////////////////
   void LibrarySharingManager::AddToSearchPath(const string& newPath)
   {
      mSearchPath.insert(newPath);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void LibrarySharingManager::RemoveFromSearchPath(const string& path)
   {
      set<string>::iterator i = mSearchPath.find(path);
      if (i != mSearchPath.end())
      {
         mSearchPath.erase(i);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void LibrarySharingManager::ClearSearchPath()
   {
      mSearchPath.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   string LibrarySharingManager::GetPlatformSpecificLibraryName(const string &libBase, bool assumeModule)
   {
#if defined DELTA_WIN32
   #ifdef _DEBUG
      return libBase + "d.dll";
   #else
      return libBase + ".dll";
   #endif
#elif defined(__APPLE__)
      if (assumeModule)
      {
         return "lib" + libBase + ".so";
      }
      else
      {
         return "lib" + libBase + ".dylib";
      }
#else
      return "lib" + libBase + ".so";
#endif
   }

   ///////////////////////////////////////////////////////////////////////////////
   string LibrarySharingManager::GetPlatformIndependentLibraryName(const string &libName)
   {
      string iName = osgDB::getStrippedName(libName);

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BCPLUSPLUS__)  || defined(__MWERKS__)
   #ifdef _DEBUG
      //Pull off the final "d"
      return string(iName.begin(),iName.end() - 1);
   #else
      return iName;
   #endif
#else
      return string(iName.begin()+3,iName.end());
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   LibrarySharingManager::LibraryLoadingException::LibraryLoadingException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }
} // namespace dtUtil
