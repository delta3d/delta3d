#include <dtDIS/pluginmanager.h>             // for class skeleton
#include <dtUtil/log.h>                      // for debugging
#include <dtUtil/librarysharingmanager.h>    // for loading/unloading libraries
#include <osgDB/FileNameUtils>               // for file extension stripping code
#include <dtUtil/stringutils.h>              // for printing with ToString

using namespace dtDIS;

bool PluginManager::LoadPlugin(const std::string& path)
{
   // load the library
   dtUtil::LibrarySharingManager& lsm = dtUtil::LibrarySharingManager::GetInstance();
   std::string fileName = osgDB::getSimpleFileName(path);
   std::string dir = osgDB::getFilePath(path);
   lsm.AddToSearchPath(dir);

   std::string candidate = lsm.GetPlatformIndependentLibraryName(fileName);
   
   // ugly correction for library loading tool.
#ifdef _DEBUG
   if( *(candidate.rbegin()) == 'd' )   // if last letter == 'd'
   {
      std::string candidate2( candidate.begin() , (--candidate.end()) );
      candidate = candidate2;
   }
#endif

   dtCore::RefPtr<dtUtil::LibrarySharingManager::LibraryHandle> libhandle = lsm.LoadSharedLibrary( candidate );

   return LoadLibraryHandle( path, libhandle.get() );
}


bool PluginManager::LoadLibraryHandle(const std::string& path, dtUtil::LibrarySharingManager::LibraryHandle* lib)
{
   // find the symbols in the library
   RegistryEntry reg;
   if( mLoadStrategy.LoadSymbols( lib, reg ) )
   {
      reg.mHandle = lib;
      reg.mCreated = reg.mCreator();

      // add the entry object to the container
      if( mPlugins.insert( LibraryRegistry::value_type( path , reg ) ).second )
      {
         mLoaded( path , reg );
         return true;
      }
      else
      {
         LOG_INFO("Library already loaded with path: " + path )
         return false;
      }
   }

   LOG_INFO("Failed to load plugin: " + path)
   return false;
}

bool PluginManager::UnloadPlugin(const std::string& path)
{
   LibraryRegistry::iterator iter = mPlugins.find( path );
   if( iter != mPlugins.end() )
   {
      UnloadImplementation( iter );
      return true;
   }

   LOG_INFO("Failed to unload plugin: " + path)
   return false;
}

void PluginManager::UnloadImplementation(LibRegIter& iter)
{
   mUnloading( iter->first , iter->second );

   (iter->second).mDestroyer( (iter->second).mCreated );
   (iter->second).mCreated = NULL;

   // unloading the library happens when
   // there are no references to the mHandle member of the LibaryEntryType.
   // remove entry from the container so that there are no more references to the library model.
   mPlugins.erase( iter );
}

PluginManager::PluginSignal& PluginManager::GetLoadedSignal()
{
   return mLoaded;
}

PluginManager::PluginSignal& PluginManager::GetUnloadedSignal()
{
   return mUnloading;
}

const PluginManager::LibraryRegistry& PluginManager::GetRegistry() const
{
   return mPlugins;
}

PluginManager::LibraryRegistry& PluginManager::GetRegistry()
{
   return mPlugins;
}

void PluginManager::UnloadAllPlugins()
{
   size_t psize = mPlugins.size();
   LOG_DEBUG("Attempting to unload " + dtUtil::ToString( psize ) + " plugins.")

   while( !mPlugins.empty() )
   {
      LibRegIter iter = mPlugins.begin();
      UnloadImplementation( iter );
   }
}
